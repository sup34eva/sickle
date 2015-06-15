// Copyright 2015 PsychoLama

#include <light.hpp>
#include <viewport.hpp>

Light::Light(QObject* parent) : Actor(parent) {
	nearZ(-63);
	farZ(66);
	power(1.0);
	color(QColor(255, 255, 255));
	mapSize(QSize(140, 103));
	orientation(fromEuler(QVector3D(-45, 0, -45)));
	setObjectName(tr("New Light"));

	connect(this, &Light::nearZChanged, this, &Light::update);
	connect(this, &Light::farZChanged, this, &Light::update);
	connect(this, &Light::mapSizeChanged, this, &Light::update);
	connect(this, &Light::moved, this, &Light::update);
	connect(this, &Light::rotated, this, &Light::update);

	update();
	if(parent != nullptr)
		setParent(parent);
}

Light::~Light() {
	auto view = viewport();
	if(view != nullptr) view->removeLight(this);
}

void Light::updateProj() {
	m_projection.setToIdentity();
	auto size = m_mapSize / 2;
	m_projection.ortho(-size.width(), size.width(), -size.height(), size.height(), m_nearZ, m_farZ);
}

void Light::updateView() {
	m_view.setToIdentity();
	m_view.lookAt(direction(), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
}

void Light::update() {
	auto view = viewport();

	updateProj();
	updateView();

	if(view != nullptr)
		view->updateLight(this);
}

void Light::setParent(QObject* parent) {
	Actor::setParent(parent);
	auto view = viewport();
	if(view != nullptr) {
		view->initLight(*this);
		view->updateLight(this);
	}
}

QVector3D Light::direction() {
	auto dir = orientation().toVector4D();
	return QVector3D(    2 * (dir.x() * dir.z() + dir.w() * dir.y()),
						 2 * (dir.y() * dir.x() - dir.w() * dir.x()),
					 1 - 2 * (dir.x() * dir.x() + dir.y() * dir.y())).normalized();
}

QMatrix4x4 Light::depth() {
	return m_projection * m_view;
}
