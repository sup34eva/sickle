#include <viewport.hpp>

Viewport::Viewport(QWidget* parent)
	: QOpenGLWidget(parent),
	  m_renderMode(GL_TRIANGLES)  // GL_LINES
{
	resizeGL(width(), height());
	m_camera = new Camera();
	setFocusPolicy(Qt::StrongFocus);
}

Viewport::~Viewport() {
	delete m_camera;
}

QSize Viewport::minimumSizeHint() const {
	return QSize(500, 500);
}

QSize Viewport::sizeHint() const {
	return QSize(500, 500);
}

void Viewport::initializeGL() {
	initializeOpenGLFunctions();
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_CULL_FACE);
}

void Viewport::paintGL() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	QMatrix4x4 view = m_camera->view();

	DrawInfo info{view, m_projection, m_renderMode};
	for (auto i : children()) {
		auto child = dynamic_cast<Geometry*>(i);
		if (child) child->draw(info);
	}
}

void Viewport::resizeGL(int w, int h) {
	if (QOpenGLFunctions::isInitialized(QOpenGLFunctions::d_ptr)) glViewport(0, 0, w, h);
	m_projection.setToIdentity();
	m_projection.perspective(45.0f, (float)w / (float)h, 0.1f, 1000.0f);
}

void Viewport::wheelEvent(QWheelEvent* event) {
	m_camera->move(m_camera->direction() * (event->delta() / 120));
	update();
	event->accept();
}

void Viewport::keyPressEvent(QKeyEvent* event) {
	switch (event->key()) {
		case Qt::Key_Up:
			m_camera->move(m_camera->direction());
			break;
		case Qt::Key_Down:
			m_camera->move(-m_camera->direction());
			break;
		case Qt::Key_Right:
			m_camera->move(m_camera->right());
			break;
		case Qt::Key_Left:
			m_camera->move(-m_camera->right());
			break;
		case Qt::Key_PageUp:
			m_camera->move(m_camera->up());
			break;
		case Qt::Key_PageDown:
			m_camera->move(-m_camera->up());
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
	auto deltaX = ((size().width() / 2) - (float)(event->x())) / size().width();
	auto deltaY = ((size().height() / 2) - (float)(event->y())) / size().height();
	QCursor::setPos(mapToGlobal(QPoint(size().width() / 2, size().height() / 2)));
	m_camera->hAngle(m_camera->hAngle() + (deltaX * 3.14f));
	m_camera->vAngle(m_camera->vAngle() + (deltaY * 3.14f));
	update();
}

void Viewport::save(QString name) {
	QFile file(name);
	file.open(QIODevice::WriteOnly);
	QDataStream out(&file);
	out << static_cast<quint32>(0xB00B1E5);  // Magic number

	auto version = QDataStream::Qt_5_4;  // Format version
	out << static_cast<qint32>(version);
	out.setVersion(version);

	// Data
	out << *camera();
	auto childList = findChildren<Geometry*>();
	out << static_cast<quint32>(childList.size());
	for (auto obj : childList) {
		out << *obj;
	}
}

void Viewport::clearLevel() {
	auto childList = findChildren<Geometry*>();
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

	qint32 version;  // Format version
	in >> version;
	in.setVersion(version);

	clearLevel();

	// Data
	in >> *camera();
	quint32 size;
	in >> size;
	qDebug() << size;
	for (quint32 i = 0; i < size; i++) {
		Geometry* obj = addChild();
		in >> *obj;
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
