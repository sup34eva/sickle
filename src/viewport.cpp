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

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), m_isInitialized(false) {
	m_camera = new Camera(this);
	m_world = new World(this);

	setFocusPolicy(Qt::StrongFocus);

	qRegisterMetaType<Cube>("Cube");
	qRegisterMetaType<Sphere>("Sphere");
	qRegisterMetaType<Pyramide>("Pyramide");
	qRegisterMetaType<Cylinder>("Cylinder");

	qRegisterMetaType<Line>("Line");

	qRegisterMetaType<Light>("Light");
	qRegisterMetaType<Spotlight>("Spotlight");
	qRegisterMetaType<Group>("Group");
	qRegisterMetaType<Material>("Material");
	qRegisterMetaType<Trigger>("Trigger");

	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(16);
	format.setStencilBufferSize(8);
	format.setSamples(16);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	setFormat(format);
}

void Viewport::initLight(Light& light) {
	makeCurrent();
	glGenFramebuffers(1, &light.getBuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, light.getBuffer());

	static const GLfloat colors[] = {0, 0, 0};

	glGenTextures(1, &light.getTexture());
	glBindTexture(GL_TEXTURE_2D, light.getTexture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, colors);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	doneCurrent();
}

void Viewport::initScene() {
	glGenFramebuffers(1, &m_sceneBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);

	const int kNum = 9;
	GLuint tex[kNum];
	glGenTextures(kNum, tex);
	for(int i = 0; i < kNum; i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_sceneTextures.append(tex[i]);
	}

	glGenRenderbuffers(1, &m_sceneDepth);
}

void Viewport::initQuad() {
	glGenVertexArrays(1, &m_quadVAO);
	glBindVertexArray(m_quadVAO);

	static const GLfloat data[] = {
		-1.0f, -1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
		 1.0f, -1.0f, 0.0f,
		 1.0f,  1.0f, 0.0f,
	};

	glGenBuffers(1, &m_quadBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STATIC_DRAW);

	static const QList<QString> list = {
		":/shaders/unlit.frag",
		":/shaders/light.frag",
		":/shaders/ambient.frag"
	};

	for(int i = 0; i < list.length(); i++) {
		auto prog = new QOpenGLShaderProgram(this);
		prog->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/deferred.vert");
		prog->addShaderFromSourceFile(QOpenGLShader::Fragment, list.at(i));
		prog->link();
		m_quadPrograms.insert(i, prog);
	}
}

void Viewport::initializeGL() {
	initializeOpenGLFunctions();

	showBuffers(false);
	showMaps(false);
	ambient(QColor(50, 50, 50));
	program(tr("Light"));

	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

	initScene();
	initQuad();

	m_bgColor = palette().color(QPalette::Background);

	catchErrors();

	m_world->addZone();

	isInitialized(true);

	qDebug() << "OpenGL version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

void Viewport::catchErrors() {
	auto error = glGetError();
	if(error != 0) {
		qFatal("GL Errors: %s", reinterpret_cast<const char*>(glGetString(error)));
	}
}

void Viewport::renderLight(Light* light) {
	glBindFramebuffer(GL_FRAMEBUFFER, light->getBuffer());
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->getTexture(), 0);
	catchErrors();

	glDrawBuffer(GL_NONE);

	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);
	catchErrors();

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Light buffer status:" << status;
		return;
	}

	glCullFace(GL_FRONT);

	QVariantHash uniforms;
	uniforms.insert("model", QMatrix4x4());
	uniforms.insert("projection", light->projection());
	uniforms.insert("view", light->view());

	DrawInfo info = {GL_TRIANGLES, context(), RB_DEPTH, uniforms};
	m_world->currentZone()->draw(info);
	catchErrors();
}

void Viewport::renderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_sceneDepth);
	catchErrors();

	auto len = m_sceneTextures.length();
	QVector<GLenum> buffers;
	for(int i = 0; i < len; i++) {
		GLenum attachment = i > 7 ? GL_DEPTH_ATTACHMENT : GL_COLOR_ATTACHMENT0 + i;
		glFramebufferTexture(GL_FRAMEBUFFER, attachment, m_sceneTextures.at(i), 0);
		if(i <= 7) buffers.append(attachment);
	}

	glDrawBuffers(len - 1, &buffers.at(0));

	glViewport(0, 0, width(), height());
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	catchErrors();

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Scene buffer status:" << status;
		return;
	}

	glCullFace(GL_BACK);
	catchErrors();

	QVariantHash uniforms;
	uniforms.insert("model", QMatrix4x4());
	uniforms.insert("projection", m_projection);
	uniforms.insert("view", m_camera->view());

	GLenum mode = m_program == tr("Wireframe") ? GL_LINES : GL_TRIANGLES;
	DrawInfo info = {mode, context(), RB_SCENE, uniforms};

	m_world->currentZone()->draw(info);
}

QVector3D toVector(const QColor& col) {
	return QVector3D(col.redF(), col.greenF(), col.blueF());
}

void Viewport::renderQuad() {
	glBindVertexArray(m_quadVAO);
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	glDisable(GL_DEPTH_TEST);

	glClearColor(m_bgColor.redF(), m_bgColor.greenF(), m_bgColor.blueF(), m_bgColor.alphaF());
	glClear(GL_COLOR_BUFFER_BIT);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Main buffer status:" << status;
		return;
	}

	if(m_showBuffers || m_showMaps) {
		auto prog = m_quadPrograms.value(0);
		prog->bind();

		int len;
		QList<Light*> lights;
		if(m_showBuffers) {
			len = m_sceneTextures.length();
		} else {
			lights = m_world->currentZone()->findChildren<Light*>();
			len = lights.length();
		}

		if(len < 1) {
			glViewport(0, 0, width(), height());
		} else {
			int div = 1, w, h;
			while(pow(div, 2) < len + 1) div++;
			w = width() / div;
			h = height() / div;

			for(int i = 0; i <= len; i++) {
				int x = i % div,
					y = (div - 1) - (i / div);
				glViewport(x * w, y * h, w, h);
				if(i < len) {
					glActiveTexture(GL_TEXTURE0);
					if(m_showBuffers) {
						glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
					} else {
						glBindTexture(GL_TEXTURE_2D, lights.at(i)->getTexture());
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);
					}
					glEnableVertexAttribArray(0);
					glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
					glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
					glDrawArrays(GL_TRIANGLES, 0, 6);
					glDisableVertexAttribArray(0);
					if(m_showMaps)
						glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
				}
			}
			prog->release();
		}
	} else {
		glViewport(0, 0, width(), height());
	}

	int lightsLen = 1;
	QList<Light*> lightList;
	bool isLit = m_program == tr("Light");
	if(isLit) {
		lightList = m_world->currentZone()->findChildren<Light*>();
		lightsLen = lightList.length() + 1;
	}

	for(int i = 0; i < lightsLen; i++) {
		bool isAmbient = isLit && i == 0;
		bool isLight = isLit && !isAmbient;

		int id;
		if (isAmbient)
			id = 2;
		else if (isLight)
			id = 1;
		else
			id = 0;

		auto prog = m_quadPrograms.value(id);
		prog->bind();

		Light* light = nullptr;
		if(isLight)
			light = lightList.value(i - 1);

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
			if(isAmbient) {
				prog->setUniformValue("ambientColor", toVector(m_ambient));
				auto occlusion = qobject_cast<AmbientOcclusion*>(m_world->AO());
				prog->setUniformValue("AO.threshold", occlusion->threshold());
				prog->setUniformValue("AO.kernelSize", QVector2D(
										  occlusion->kernelSize() / width(),
										  occlusion->kernelSize() / height()));
				prog->setUniformValue("AO.maxDist", occlusion->maxDist());
			} else {
				auto type = light->type();
				prog->setUniformValue("light.type", type);
				prog->setUniformValue("light.orientation", light->direction());
				prog->setUniformValue("light.location", light->position());
				prog->setUniformValue("light.power", light->power());
				prog->setUniformValue("light.color", toVector(light->color()));

				if(type == 1) {
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

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		prog->release();

		if(isAmbient) {
			glBlendEquation(GL_FUNC_ADD);
			glBlendFunc(GL_ONE, GL_ONE);
		}
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Viewport::paintGL() {
	catchErrors();

	for(auto i : m_dirtyLights)
		renderLight(i);

	catchErrors();
	renderScene();
	catchErrors();
	renderQuad();

	catchErrors();

	m_dirtyLights.clear();
}

void Viewport::resizeGL(int w, int h) {
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.0f);
	for(int i = 0; i < m_sceneTextures.length(); i++) {
		glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
		GLint internal = i > 7 ? GL_DEPTH_COMPONENT32F : GL_RGBA32F_ARB;
		GLenum format = i > 7 ? GL_DEPTH_COMPONENT : GL_RGBA;
		glTexImage2D(GL_TEXTURE_2D, 0, internal, w, h, 0, format, GL_FLOAT, 0);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, w, h);
	catchErrors();
}

void Viewport::wheelEvent(QWheelEvent* event) {
	m_camera->move(m_camera->direction() * (event->delta() / 120));
	update();
	event->accept();
}

void Viewport::updateLights() {
	if(m_isInitialized) {
		auto zone = m_world->currentZone();
		auto lights = zone->findChildren<Light*>();
		foreach(auto light, lights) updateLight(light);
	}
}

void Viewport::updateLight(Light* light) {
	if(m_dirtyLights.indexOf(light) == -1)
		m_dirtyLights.append(light);
}

void Viewport::removeLight(Light* light) {
	m_dirtyLights.removeAll(light);
}

void Viewport::keyPressEvent(QKeyEvent* event) {
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
			event->ignore();
	}
	if (event->isAccepted()) update();
}

void Viewport::mousePressEvent(QMouseEvent* event) {
	event->accept();
	grabMouse();
	setCursor(QCursor(Qt::BlankCursor));
	m_cursor = event->globalPos();
	QCursor::setPos(mapToGlobal(QPoint(size().width() / 2, size().height() / 2)));
	m_camera->resetBias();
}

void Viewport::mouseReleaseEvent(QMouseEvent* event) {
	event->accept();
	releaseMouse();
	QCursor::setPos(m_cursor);
	setCursor(QCursor(Qt::ArrowCursor));
}

void Viewport::mouseMoveEvent(QMouseEvent* event) {
	event->accept();
	auto deltaX = ((size().width() / 2) - static_cast<float>(event->x())) / size().width();
	auto deltaY = ((size().height() / 2) - static_cast<float>(event->y())) / size().height();
	QCursor::setPos(mapToGlobal(QPoint(size().width() / 2, size().height() / 2)));
	m_camera->hAngle(m_camera->hAngle() + (deltaX * 3.14f));
	m_camera->vAngle(m_camera->vAngle() + (deltaY * 3.14f));
	update();
}

void Viewport::clearLevel() {
	for (auto obj : findChildren<Actor*>()) {
		if(obj != nullptr) {
			obj->deleteLater();
		}
	}
}
