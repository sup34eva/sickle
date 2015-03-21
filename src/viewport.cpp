// Copyright 2015 PsychoLama

#include <viewport.hpp>
#include <QStyle>

Viewport::Viewport(QWidget* parent) : QOpenGLWidget(parent), m_renderMode(GL_TRIANGLES) {
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	auto view = m_camera->view();
	DrawInfo info{view, m_projection, m_renderMode, context()};

	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

void Viewport::resizeGL(int w, int h) {
	if (QOpenGLFunctions::isInitialized(QOpenGLFunctions::d_ptr)) glViewport(0, 0, w, h);
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

void Viewport::clearLevel() {
	auto childList = findChildren<Actor*>();
	for (auto obj : childList) {
		delete obj;
	}
}
