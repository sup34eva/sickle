// Copyright 2015 PsychoLama

#include <viewport.hpp>
#include <QStyle>
#include <light.hpp>
#include <spotlight.hpp>
#include <group.hpp>
#include <pyramide.hpp>
#include <cylinder.hpp>
#include <trigger.hpp>
#include <line.hpp>
#include <rectangle.hpp>

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), m_isInitialized(false) {
	// Création de la caméra et du monde
	m_camera = new Camera(this);
	m_world = new World(this);

	// Relaie les evenements du monde
	m_relay = connect(m_world, &World::zoneAdded, this, &Viewport::zoneAdded);

	// Recupère activeent le focus de l'utilisateur
	setFocusPolicy(Qt::StrongFocus);

	// Requiert un contexte OpenGL
	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(16);
	format.setStencilBufferSize(8);
	format.setSamples(16);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	setFormat(format);
}

void Viewport::initLight(Light& light) {
	// Active le contexte OpenGL
	makeCurrent();

	// Crée le framebuffer de la lumière
	glGenFramebuffers(1, &light.getBuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, light.getBuffer());

	// Coulleur servant a remlpir les bords de la texture
	static const GLfloat colors[] = {0, 0, 0};

	// Créée la textre du framebuffer
	glGenTextures(1, &light.getTexture());

	// Charge la texture
	glBindTexture(GL_TEXTURE_2D, light.getTexture());

	// Crée une texture de 1024 x 1024
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);

	// Désactive le filtrage
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	// Désactive la répétition de texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);

	// Active la comparaison avec les valeurs float
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	catchErrors();
}

void Viewport::initScene() {
	// Créé et charge le framebuffer de la scène
	glGenFramebuffers(1, &m_sceneBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);

	// Génère les textures de tous les composants
	const int kNum = 9;
	GLuint tex[kNum];
	glGenTextures(kNum, tex);

	for(int i = 0; i < kNum; i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);

		// Active le filtrage
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

		// Désactive la répétion de texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

		m_sceneTextures.append(tex[i]);
	}

	// Crée le renderbuffer
	glGenRenderbuffers(1, &m_sceneDepth);
}

void Viewport::initQuad() {
	// Crée un VAO pour le quad fullscreen
	glGenVertexArrays(1, &m_quadVAO);
	glBindVertexArray(m_quadVAO);

	// Vertices du quad
	static const GLfloat data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	// Buffer pour le quad
	glGenBuffers(1, &m_quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	// Shaders du quad
	static const QList<QString> list = {
		":/shaders/unlit.frag",
		":/shaders/light.frag",
		":/shaders/ambient.frag"
	};

	// Initialisation des shaders
	for(int i = 0; i < list.length(); i++) {
		auto prog = new QOpenGLShaderProgram(this);
		prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/deferred.vert");
		prog->addShaderFromSourceFile(QOpenGLShader::Fragment, list.at(i));
		prog->link();
		m_quadPrograms.insert(i, prog);
	}
}

void Viewport::initializeGL() {
	// Charge les fonctions OpenGL
	initializeOpenGLFunctions();

	// Initialise les propriétés du Viewport
	showBuffers(false);
	showMaps(false);
	ambient(QColor(50, 50, 50));
	program(tr("Light"));

	// Active les textures, le depth sorting et le backface culling
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	// Initialise la scene et le quad fullscreen
	initScene();
	initQuad();

	// Charge la couleur d'arrière plan depuis le CSS
	m_bgColor = palette().color(QPalette::Background);

	// Ajoute une zone de base
	m_world->addZone();

	// Marque le Viewport comme initialisé
	isInitialized(true);
}

void Viewport::catchErrors() {
	auto error = glGetError();
	if(error != 0) {
		qFatal("GL Errors: %s", reinterpret_cast<const char*>(glGetString(error)));
	}
}

void Viewport::renderLight(Light* light) {
	// Active le framebuffer de la lumière
	glBindFramebuffer(GL_FRAMEBUFFER, light->getBuffer());
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->getTexture(), 0);
	glDrawBuffer(GL_NONE);

	// Initialise le viewport
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);

	// Vérifie l'état du framebuffer
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Light buffer status:" << status;
		return;
	}

	// Inverse le backface culling pour le rendu des shadowmaps
	glCullFace(GL_FRONT);

	// Paramètres du shader
	QVariantHash uniforms;
	uniforms.insert("model", QMatrix4x4());
	uniforms.insert("projection", light->projection());
	uniforms.insert("view", light->view());

	// Paramètres de la frame
	DrawInfo info = {GL_TRIANGLES, context(), RB_DEPTH, uniforms};

	// Affiche la scène
	m_world->currentZone()->draw(info);

	catchErrors();
}

void Viewport::renderScene() {
	// Active le framebuffer de la scène
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_sceneDepth);

	// Lie les attachments aux textures
	auto len = m_sceneTextures.length();
	QVector<GLenum> buffers;
	for(int i = 0; i < len; i++) {
		GLenum attachment = i > 7 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_sceneTextures.at(i), 0);
		if(i <= 7) buffers.append(attachment);
	}

	// Affiche la scène dans les buffers
	glDrawBuffers(len - 1, &buffers.at(0));

	// Initialise le viewport
	glViewport(0, 0, width(), height());
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	// Vérifie l'état du framebuffer
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Scene buffer status:" << status;
		return;
	}

	// Réinitialise le backface culling
	glCullFace(GL_BACK);

	// Paramètres du shader
	QVariantHash uniforms;
	uniforms.insert("model", QMatrix4x4());
	uniforms.insert("projection", m_projection);
	uniforms.insert("view", m_camera->view());

	// Paramètres de la frame
	GLenum mode = m_program == tr("Wireframe") ? GL_LINES : GL_TRIANGLES;
	DrawInfo info = {mode, context(), RB_SCENE, uniforms};

	// Affiche la scène
	m_world->currentZone()->draw(info);
}

//! Convertit une couleur en vecteur
QVector3D toVector(const QColor& col) {
	return QVector3D(col.redF(), col.greenF(), col.blueF());
}

void Viewport::renderQuad() {
	// Active le VAO du quad
	glBindVertexArray(m_quadVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());

	// Desactive le test de profondeur
	glDisable(GL_DEPTH_TEST);

	// Vide l'écran
	glClearColor(m_bgColor.redF(), m_bgColor.greenF(), m_bgColor.blueF(), m_bgColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT);

	// Active le blending en fonction du canal alpha
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Vérifie le statut du framebuffer
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Main buffer status:" << status;
		return;
	}

	// Si l'utilisateur a choisi d'afficher une vue de débogage
	if(m_showBuffers || m_showMaps) {
		// Charge le shader "Unlit"
		auto prog = m_quadPrograms.value(0);
		prog->bind();

		// Détermine la quantité de buffers a afficher
		int len;
		QList<Light*> lights;
		if(m_showBuffers) {
			len = m_sceneTextures.length();
		} else {
			lights = m_world->currentZone()->findChildren<Light*>();
			len = lights.length();
		}

		// Si il y a plus de 2 buffers
		if(len < 1) {
			glViewport(0, 0, width(), height());
		} else {
			// Calcule en combien le viewport doit être divisé
			int div = 1, w, h;
			while(pow(div, 2) < len + 1) div++;
			w = width() / div;
			h = height() / div;

			for(int i = 0; i <= len; i++) {
				int x = i % div,
					y = (div - 1) - (i / div);

				// Initialise le viewport
				glViewport(x * w, y * h, w, h);
				if(i < len) {
					// Charge le buffer dans la texture 0
					glActiveTexture(GL_TEXTURE0);
					if(m_showBuffers) {
						glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
					} else {
						glBindTexture(GL_TEXTURE_2D, lights.at(i)->getTexture());

						// Déastive le mode de comparaison pour afficher les shadowmaps
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
					}

					// Charge le VAO
					glEnableVertexAttribArray(0);

					// Active le buffer
					glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);

					// Active l'attribut de position
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);

					// Affiche le quad
					glDrawArrays(GL_TRIANGLES, 0, 6);

					// Desactive le VAO
					glDisableVertexAttribArray(0);

					// Si les shadowmap sont affichée, retabli le mode de comparaison
					if(m_showMaps)
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				}
			}

			// Décharge le shader
			prog->release();
		}
	} else {
		// Sinon, configure le viewport normalement
		glViewport(0, 0, width(), height());
	}

	// Si le mode de rendu est "Eclairé", charge la liste des lumières
	int lightsLen = 1;
	QList<Light*> lightList;
	bool isLit = m_program == tr("Light");
	if(isLit) {
		lightList = m_world->currentZone()->findChildren<Light*>();
		lightsLen = lightList.length() + 1;
	}

	// Pour chaque lumière
	for(int i = 0; i < lightsLen; i++) {
		bool isAmbient = isLit && i == 0;
		bool isLight = isLit && !isAmbient;

		// Détermine le shader a utiliser pour cette passe
		int id;
		if (isAmbient)
			id = 2;
		else if (isLight)
			id = 1;
		else
			id = 0;

		// Charge le shader
		auto prog = m_quadPrograms.value(id);
		prog->bind();

		Light* light = nullptr;
		if(isLight)
			light = lightList.value(i - 1);

		// Liste des buffers a transmettre au shader
		static const QList<QString> names = {
			"color",
			"normal",
			"tangent",
			"bitangent",
			"vertPos",
			"matProp1",
			"matProp2",
			"matProp3",
			"depth",
			"shadowMap"
		};

		// Charge les buffers
		int maxLen = m_sceneTextures.length();
		int texLen = 1;
		if(isLit) texLen = maxLen;
		if(isLight) texLen++;
		for(int j = 0; j < texLen; j++) {
			glActiveTexture(GL_TEXTURE0 + j);
			auto tex = (isLight && j >= maxLen) ? light->getTexture() : m_sceneTextures.at(j);
			glBindTexture(GL_TEXTURE_2D, tex);
			auto n = names.at(j);
			auto loc = prog->uniformLocation(n);
			if(loc > -1)
				prog->setUniformValue(loc, j);
#ifdef DEBUG_SHADERS  // Les uniforms sont souvent supprimés lors de l'optimisation des shaders durant leur compilation
			else
				qWarning().noquote() << "Uniform" << n << "not found.";
#endif
		}


		if(isLit) {
			// Si il d'agit d'une passe d'ambiance
			if(isAmbient) {
				// Passe au shader la couleur ambiante et d'ambient occlusion
				prog->setUniformValue("ambientColor", toVector(m_ambient));
				auto occlusion = qobject_cast<AmbientOcclusion*>(m_world->AO());
				prog->setUniformValue("AO.threshold", occlusion->threshold());
				prog->setUniformValue("AO.kernelSize", QVector2D(
										  occlusion->kernelSize() / width(),
										  occlusion->kernelSize() / height()));
				prog->setUniformValue("AO.maxDist", occlusion->maxDist());
			} else {
				// Lors d'une passe de lumière, passe les informations de la lumière au shader
				auto type = light->type();
				prog->setUniformValue("light.type", type);
				prog->setUniformValue("light.orientation", light->direction());
				prog->setUniformValue("light.location", light->position());
				prog->setUniformValue("light.power", light->power());
				prog->setUniformValue("light.color", toVector(light->color()));

				if(type == 1) { // Spotlight
					auto spot = static_cast<Spotlight*>(light);
					prog->setUniformValue("light.falloff", spot->farZ());
					prog->setUniformValue("light.inner", static_cast<GLfloat>(qCos(qDegreesToRadians(spot->innerAngle()))));
					prog->setUniformValue("light.outer", static_cast<GLfloat>(qCos(qDegreesToRadians(spot->outerAngle()))));
				}

				prog->setUniformValue("vDepth", light->view());
				prog->setUniformValue("pDepth", light->projection());
				prog->setUniformValue("eyeD", -m_camera->direction().normalized());
			}
		}

		// Affiche le quad fullscreen
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		// Décharge le shader
		prog->release();

		if(isAmbient) {
			// Après une passe d'ambiance, modifie le mode de synthèse en additif pour les passes de lumière suivantes
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
		}
	}

	// Désactive la synthèse et réactive le test de profondeur
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Viewport::paintGL() {
	// Recalcule toutes les shadowmaps ayant changé
	for(auto i : m_dirtyLights)
		renderLight(i);

	// Enregistre la scène
	renderScene();

	// Affiche le quad
	renderQuad();

	// Vide la liste des lumières modifiées
	m_dirtyLights.clear();
}

void Viewport::resizeGL(int w, int h) {
	// Recalcule la matrice de projection
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.0f);

	// Redimensionne tous les buffers
	for(int i = 0; i < m_sceneTextures.length(); i++) {
		glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
		GLint internal = i > 7 ? GL_DEPTH_COMPONENT32F : GL_RGBA32F_ARB;
		GLenum format = i > 7 ? GL_DEPTH_COMPONENT : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, format, GL_FLOAT, 0);
	}

	glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
}

void Viewport::wheelEvent(QWheelEvent* event) {
	// Déplace la caméra en fonction de la distance parcure par la roue de la souris
	m_camera->move(m_camera->direction() * (event->delta() / 120));

	// Met a jour l'image
	update();

	// Accepte l'évenement
	event->accept();
}

void Viewport::updateLights() {
	// Si le monde est initialisé
	if(m_isInitialized && m_world->zoneList().size() > 0) {
		// Liste toutes les lumières dans la zone courante
		auto zone = m_world->currentZone();
		auto lights = zone->findChildren<Light*>();

		// Ajoute chaque lumière a la liste des lumières modifiées
		foreach(auto light, lights) updateLight(light);
	}
}

void Viewport::updateLight(Light* light) {
	// Ajoute la lumière uniquement si est n'est pas encore dans la liste
	if(m_dirtyLights.indexOf(light) == -1)
		m_dirtyLights.append(light);
}

void Viewport::removeLight(Light* light) {
	m_dirtyLights.removeAll(light);
}

void Viewport::keyPressEvent(QKeyEvent* event) {
	// Déplace la caméra en fonction des appuis de touche
	switch (event->key()) {
		case Qt::Key_Up:
			m_camera->velocity(m_camera->direction());
			break;
		case Qt::Key_Down:
			m_camera->velocity(-m_camera->direction());
			break;
		case Qt::Key_Right:
			m_camera->velocity(m_camera->right());
			break;
		case Qt::Key_Left:
			m_camera->velocity(-m_camera->right());
			break;
		case Qt::Key_PageUp:
			m_camera->velocity(m_camera->up());
			break;
		case Qt::Key_PageDown:
			m_camera->velocity(-m_camera->up());
			break;
		case Qt::Key_Z:
			m_camera->vAngle(m_camera->vAngle() + 0.14f);
			break;
		case Qt::Key_S:
			m_camera->vAngle(m_camera->vAngle() - 0.14f);
			break;
		case Qt::Key_Q:
			m_camera->hAngle(m_camera->hAngle() + 0.14f);
			break;
		case Qt::Key_D:
			m_camera->hAngle(m_camera->hAngle() - 0.14f);
			break;
		default:
			// Si la touche est inconnue, ignore l'évenement
			event->ignore();
	}

	// Si l'évenement a été accepté, met a jour l'image
	if (event->isAccepted()) update();
}

void Viewport::mousePressEvent(QMouseEvent* event) {
	// Accepte l'évenement
	event->accept();

	// Capture la souris
	grabMouse();

	// Masque le curseur
	setCursor(QCursor(Qt::BlankCursor));

	// Enregistre le position du curseur
	m_cursor = event->globalPos();

	// Place le curseur au centre de l'écran
	QCursor::setPos(mapToGlobal(QPoint(size().width() / 2, size().height() / 2)));

	// Réinitialise le biais de la caméra
	m_camera->resetBias();
}

void Viewport::mouseReleaseEvent(QMouseEvent* event) {
	// Accepte l'évenement
	event->accept();

	// Relache la souris
	releaseMouse();

	// Replace le curseur a se position initiale
	QCursor::setPos(m_cursor);

	// Affiche le curseur
	setCursor(QCursor(Qt::ArrowCursor));
}

void Viewport::mouseMoveEvent(QMouseEvent* event) {
	// Accepte l'évenement
	event->accept();

	// Calcule la distance parcourue par le curseur
	auto deltaX = ((size().width() / 2) - static_cast<float>(event->x())) / size().width();
	auto deltaY = ((size().height() / 2) - static_cast<float>(event->y())) / size().height();

	// Replace le curseur au centre de l'écran
	QCursor::setPos(mapToGlobal(QPoint(size().width() / 2, size().height() / 2)));

	// Tourne la caméra en fonction des deltas calculés plus haut
	m_camera->hAngle(m_camera->hAngle() + (deltaX * 3.14f));
	m_camera->vAngle(m_camera->vAngle() + (deltaY * 3.14f));

	// Met a jour l'image
	update();
}

void Viewport::clearLevel() {
	// Sauvegarde le monde actuel
	auto oldWorld = m_world;

	// Déconnecte le relai de l'ancien monde
	disconnect(m_relay);

	// Crée un nouveau monde
	m_world = new World(this);

	// Reconnecte le relai au nouveau monde
	m_relay = connect(m_world, &World::zoneAdded, this, &Viewport::zoneAdded);

	// Ajoute une zone au monde nouvellement créé
	m_world->addZone();

	// Marque l'ancien monde pour suppression
	oldWorld->deleteLater();
}
