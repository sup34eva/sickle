// Copyright 2015 PsychoLama

#include <spotlight.hpp>
#include <viewport.hpp>

Spotlight::Spotlight(QObject* parent) : Light(parent) {
	nearZ(2.0f);
	farZ(50.0f);
	outerAngle(90.0f);
	innerAngle(45.0f);
	setObjectName(tr("New Spotlight"));

	connect(this, &Spotlight::outerAngleChanged, this, &Light::update);
	connect(this, &Spotlight::innerAngleChanged, this, &Light::update);
}

void Spotlight::updateView() {
	m_view.setToIdentity();
	m_view.lookAt(position(), position() - direction(), QVector3D(0, 1, 0));
}

void Spotlight::updateProj() {
	m_projection.setToIdentity();
	m_projection.perspective(2.0f * m_outerAngle, 1.0f, nearZ(), farZ());
}
