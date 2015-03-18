// Copyright 2015 PsychoLama

#include <light.hpp>
#include <viewport.hpp>

Light::Light(QObject* parent) : Actor(parent) {
	nearZ(-10);
	farZ(20);
	power(1.0);
	color(QColor(255, 255, 255));
	mapSize(QSize(20, 20));
	QVector3D up(1, 0, 0);
	QVector3D dir(1, 1, 1);
	orientation(QQuaternion(QVector3D::dotProduct(up, dir), QVector3D::crossProduct(up, dir)).normalized());
	setObjectName(tr("New Light"));

	connect(this, &Light::nearZChanged, this, &Light::updateProj);
	connect(this, &Light::farZChanged, this, &Light::updateProj);
	connect(this, &Light::mapSizeChanged, this, &Light::updateProj);
	connect(this, &Light::moved, this, &Light::updateView);
	connect(this, &Light::rotated, this, &Light::updateView);

	update();
	if(parent != nullptr)
		setParent(parent);
}

void Light::updateProj() {
	m_projection.setToIdentity();
	auto size = m_mapSize / 2;
	m_projection.ortho(-size.width(), size.width(), -size.height(), size.height(), m_nearZ, m_farZ);
	auto view = viewport();
	if(view != nullptr)
		view->updateLight(this);
}

void Light::updateView() {
	m_view.setToIdentity();
	m_view.lookAt(direction(), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
	auto view = viewport();
	if(view != nullptr)
		view->updateLight(this);
}

void Light::update() {
	updateProj();
	updateView();
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
