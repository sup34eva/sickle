// Copyright 2015 PsychoLama

#include <viewport.hpp>
#include <QStyle>
#include <light.hpp>

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent) {
	m_camera = new Camera(this);

	setFocusPolicy(Qt::StrongFocus);

	qRegisterMetaType<Cube>("Cube");
	qRegisterMetaType<Sphere>("Sphere");
	qRegisterMetaType<Light>("Light");

	QSurfaceFormat format;
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setDepthBufferSize(16);
	format.setStencilBufferSize(8);
	format.setSamples(16);
	format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
	setFormat(format);
}

Viewport::~Viewport() {
	delete m_camera;
}

void Viewport::initLight(Light& light) {
	makeCurrent();
	glGenFramebuffers(1, light.getBuffer());
	glBindFramebuffer(GL_FRAMEBUFFER, light.buffer());

	glGenTextures(1, light.getTexture());
	glBindTexture(GL_TEXTURE_2D, light.texture());
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	doneCurrent();
}

void Viewport::initScene() {
	glGenFramebuffers(1, &m_sceneBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);

	int num = 8;
	GLuint tex[num];
	glGenTextures(num, tex);
	for(int i = 0; i < num; i++) {
		glBindTexture(GL_TEXTURE_2D, tex[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		m_sceneTextures.append(tex[i]);
	}

	glGenRenderbuffers(1, &m_sceneDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width(), height());
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

#ifdef GL_MULTISAMPLE
	glEnable(GL_MULTISAMPLE);
#endif

	initScene();
	initQuad();

	/*auto bg = palette().color(QPalette::Background);
	glClearColor(bg.redF(), bg.greenF(), bg.blueF(), bg.alphaF());*/

	auto error = glGetError();
	if(error != 0)
		qWarning() << "GL Errors:" << reinterpret_cast<const char*>(glGetString(glGetError()));

	isInitialized(true);

	qDebug() << "OpenGL version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

void Viewport::renderLight(Light* light) {
	glBindFramebuffer(GL_FRAMEBUFFER, light->buffer());
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, light->texture(), 0);

	glDrawBuffer(GL_NONE);

	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Light buffer status:" << status;
		return;
	}

	glCullFace(GL_FRONT);

	QVariantHash uniforms;
	uniforms.insert("projection", light->projection());
	uniforms.insert("view", light->view());

	DrawInfo info = {GL_TRIANGLES, context(), RB_DEPTH, uniforms};

	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

void Viewport::renderScene() {
	glBindFramebuffer(GL_FRAMEBUFFER, m_sceneBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_sceneDepth);

	auto len = m_sceneTextures.length();
	GLenum buffers[len];
	for(int i = 0; i < len; i++) {
		glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_sceneTextures.at(i), 0);
		buffers[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	glDrawBuffers(len, buffers);
	glViewport(0, 0, width(), height());
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if(status != GL_FRAMEBUFFER_COMPLETE) {
		qWarning() << "Scene buffer status:" << status;
		return;
	}

	glCullFace(GL_BACK);

	QVariantHash uniforms;
	uniforms.insert("projection", m_projection);
	uniforms.insert("view", m_camera->view());

	GLenum mode = m_program == tr("Wireframe") ? GL_LINES : GL_TRIANGLES;
	DrawInfo info = {mode, context(), RB_SCENE, uniforms};

	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

QVector3D toVector(const QColor& col) {
	return QVector3D(col.redF(), col.greenF(), col.blueF());
}

void Viewport::renderQuad() {
	glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
	glDisable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT);

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
		if(m_showBuffers)
			len = m_sceneTextures.length();
		else {
			lights = findChildren<Light*>();
			len = lights.length();
		}

		if(len <= 1)
			return;

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
				if(m_showBuffers)
					glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
				else {
					glBindTexture(GL_TEXTURE_2D, lights.at(i)->texture());
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
	} else {
		glViewport(0, 0, width(), height());
	}

	glEnable(GL_BLEND);
	glBlendEquation(GL_FUNC_ADD);
	glBlendFunc(GL_ONE, GL_ONE);

	int lightsLen = 1;
	QList<Light*> lightList;
	bool isLit = m_program == tr("Light");
	if(isLit) {
		lightList = findChildren<Light*>();
		lightsLen = lightList.length() + 1;
	}

	for(int i = 0; i < lightsLen; i++) {
		bool isAmbient = isLit && i == 0;
		bool isLight = isLit && !isAmbient;

		int id;
		if(isAmbient) id = 2;
		else if(isLight) id = 1;
		else id = 0;

		auto prog = m_quadPrograms.value(id);
		prog->bind();

		Light* light = nullptr;
		if(isLight)
			light = lightList.value(i - 1);

		static const QList<QString> names = {"color", "normal", "tangent", "bitangent", "vertPos", "matProp1", "matProp2", "matProp3", "shadowMap"};
		int texLen = 1;
		int maxLen = m_sceneTextures.length();
		if(isLight) texLen = maxLen + 1;
		for(int j = 0; j < texLen; j++) {
			glActiveTexture(GL_TEXTURE0 + j);
			auto tex = (isLight && j >= maxLen) ? light->texture() : m_sceneTextures.at(j);
			glBindTexture(GL_TEXTURE_2D, tex);
			auto n = names.at(j);
			auto loc = prog->uniformLocation(n);
			if(loc > -1)
				prog->setUniformValue(loc, j);
			else
				qWarning().noquote() << "Uniform" << n << "not found.";
		}


		if(isLit) {
			if(isAmbient) {
				prog->setUniformValue("ambientColor", toVector(m_ambient));
			} else {
				prog->setUniformValue("lightD", light->direction());
				prog->setUniformValue("vDepth", light->view());
				prog->setUniformValue("pDepth", light->projection());
				prog->setUniformValue("lightPower", light->power());
				prog->setUniformValue("lightColor", toVector(light->color()));
				prog->setUniformValue("eyeD", -m_camera->direction().normalized());
			}
		}

		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, m_quadBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(0);

		prog->release();
	}

	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void Viewport::paintGL() {
	for(auto i : m_dirtyLights)
		renderLight(i);
	renderScene();
	renderQuad();
	m_dirtyLights.clear();
}

void Viewport::resizeGL(int w, int h) {
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.0f);
	for(int i = 0; i < m_sceneTextures.length(); i++) {
		glBindTexture(GL_TEXTURE_2D, m_sceneTextures.at(i));
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, w, h, 0, GL_RGBA, GL_FLOAT, 0);
	}
	glBindRenderbuffer(GL_RENDERBUFFER, m_sceneDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width(), height());
}

void Viewport::wheelEvent(QWheelEvent* event) {
	m_camera->move(m_camera->direction() * (event->delta() / 120));
	update();
	event->accept();
}

void Viewport::updateLights() {
	m_dirtyLights.append(findChildren<Light*>());
}

void Viewport::updateLight(Light* light) {
	if(m_dirtyLights.indexOf(light) == -1)
		m_dirtyLights.append(light);
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

void Viewport::save(QString name) {
	QFile file(name);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << static_cast<quint32>(FILE_MAGIC);  // Magic number
	out << static_cast<quint32>(FILE_VERSION);  // Sickle version

	auto format = QDataStream::Qt_5_4;  // File format
	out << static_cast<qint32>(format);
	out.setVersion(format);

	// Data
	out << m_ambient;
	out << *camera();
	auto childList = findChildren<Actor*>();
	out << static_cast<quint32>(childList.size());
	for (auto obj : childList) {
		int type = QMetaType::type(obj->metaObject()->className());
		out << type;
		out << *obj;
	}
}

void Viewport::clearLevel() {
	auto childList = findChildren<Actor*>();
	for (auto obj : childList) {
		delete obj;
	}
}

void Viewport::load(QString name) {
	if(!m_isInitialized)
		return;

	QFile file(name);
	file.open(QIODevice::ReadOnly);
	QDataStream in(&file);

	quint32 magic;
	in >> magic;  // Magic number
	if(magic != FILE_MAGIC) {
		qWarning() << "Bad file format";
		return;
	}

	quint32 version;  // Sickle version
	in >> version;
	if(version != FILE_VERSION) {
		qWarning() << "Old file format";
		return;
	}

	qint32 format;  // File format
	in >> format;
	in.setVersion(format);

	clearLevel();

	// Data
	in >> m_ambient;
	in >> *camera();
	quint32 size;
	in >> size;
	for (quint32 i = 0; i < size; i++) {
		int id;
		in >> id;
		makeCurrent();
		Actor* obj = static_cast<Actor*>(QMetaType::create(id));
		qDebug() << "Restoring object of type" << QMetaType::typeName(id);
		in >> *obj;
		obj->setParent(this);
		doneCurrent();
		emit childAdded(obj);
	}
}

QDataStream& operator<<(QDataStream& stream, const QObject& obj) {
	auto metaObject = obj.metaObject();
	for (int i = 0; i < metaObject->propertyCount(); ++i) {
		auto prop = metaObject->property(i);
		qDebug() << "Saving " << prop.name();
		if (static_cast<QMetaType::Type>(prop.type()) != QMetaType::QObjectStar) stream << obj.property(prop.name());
	}
	return stream;
}

QDataStream& operator>>(QDataStream& stream, QObject& obj) {
	auto metaObject = obj.metaObject();
	for (int i = 0; i < metaObject->propertyCount(); ++i) {
		auto prop = metaObject->property(i);
		qDebug() << "Restoring " << prop.name();
		if (static_cast<QMetaType::Type>(prop.type()) != QMetaType::QObjectStar) {
			QVariant value;
			stream >> value;
			obj.setProperty(prop.name(), value);
		}
	}
	return stream;
}
