// Copyright 2015 PsychoLama

#include <camera.hpp>

Camera::Camera(QObject* parent) : QObject(parent), m_vAngle(0.0f), m_vBias(0.0f), m_hAngle(3.14f), m_hBias(0.0f),
	m_speed(2.5f) {
	position(QVector3D(0, 0, 0));
}

Camera::Camera(Camera* copy) {
	m_hAngle = copy->hAngle();
	m_hBias = copy->hBias();
	m_vAngle = copy->vAngle();
	m_vBias = copy->vBias();
	m_speed = copy->speed();
	position(copy->position());
}

void Camera::resetBias() {
	hBias(hAngle());
	vBias(vAngle());
}

QVector3D Camera::move(QVector3D val) {
	return position(m_position + val);
}

void Camera::velocity(QVector3D v, int dur) {
	QPropertyAnimation *animation = new QPropertyAnimation(this, "position");
	animation->setDuration(dur);
	animation->setStartValue(position());
	animation->setEndValue(position() + (v * speed()));
	animation->start();
}

QMatrix4x4 Camera::view() {
	QMatrix4x4 view;
	view.lookAt(position(), position() + direction(), up());
	return view;
}
