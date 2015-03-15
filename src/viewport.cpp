// Copyright 2015 PsychoLama

#include <viewport.hpp>
#include <QStyle>

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), m_renderMode(GL_TRIANGLES), m_frameBuffer(0) {
	showBuffers(false);
	nearZ(0);
	farZ(32);
	m_camera = new Camera(this);
	setFocusPolicy(Qt::StrongFocus);

	qRegisterMetaType<Cube>("Cube");
	qRegisterMetaType<Sphere>("Sphere");

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

void Viewport::initializeGL() {
	initializeOpenGLFunctions();

	glGenFramebuffers(1, &m_frameBuffer);
	glGenTextures(1, &m_depthTexture);
	glBindTexture(GL_TEXTURE_2D, m_depthTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);

#ifdef GL_MULTISAMPLE
	glEnable(GL_MULTISAMPLE);
#endif
	auto bg = palette().color(QPalette::Background);
	glClearColor(bg.redF(), bg.greenF(), bg.blueF(), bg.alphaF());

	isInitialized(true);

	qDebug() << "OpenGL version:" << reinterpret_cast<const char*>(glGetString(GL_VERSION));
}

void Viewport::paintGL() {
	if(m_showBuffers) {
		glViewport(0, 0, width() / 2, height() / 2);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, m_frameBuffer);
		glBindTexture(GL_TEXTURE_2D, m_depthTexture);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depthTexture, 0);
		glDrawBuffer(GL_NONE);
		glViewport(0, 0, 1024, 1024);

		if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
			qWarning() << "Nope";
			return;
		}
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glCullFace(GL_FRONT);

	Light light = {QVector3D(0.5, 2, 2)};

	QMatrix4x4 dP, dV;
	dP.ortho(-10, 10, -10, 10, nearZ(), farZ());
	dV.lookAt(light.orientation, QVector3D(0, 0, 0), QVector3D(0, 1, 0));

	DrawInfo info{dV, dP, m_renderMode, context(), RB_DEPTH, dP * dV, light};

	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}

	if(m_showBuffers) {
		glViewport(width() / 2, height() / 2, width() / 2, height() / 2);
	} else {
		glBindFramebuffer(GL_FRAMEBUFFER, defaultFramebufferObject());
		glViewport(0, 0, width(), height());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	glCullFace(GL_BACK);

	info.Projection = m_projection;
	info.View = m_camera->view();
	info.buffer = RB_FULL;

	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

void Viewport::resizeGL(int w, int h) {
	//if (QOpenGLFunctions::isInitialized(QOpenGLFunctions::d_ptr)) glViewport(0, 0, w, h);
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, static_cast<float>(w) / static_cast<float>(h), 0.1f, 1000.0f);
}

void Viewport::wheelEvent(QWheelEvent* event) {
	m_camera->move(m_camera->direction() * (event->delta() / 120));
	update();
	event->accept();
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
	in >> *camera();
	quint32 size;
	in >> size;
	makeCurrent();
	for (quint32 i = 0; i < size; i++) {
		int id;
		in >> id;
		auto obj = static_cast<Actor*>(QMetaType::create(id));
		qDebug() << "Restoring object of type" << QMetaType::typeName(id);
		in >> *obj;
		obj->setParent(this);
		emit childAdded(obj);
	}
	doneCurrent();
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
