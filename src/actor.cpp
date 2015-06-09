// Copyright 2015 PsychoLama

#include <actor.hpp>
#include <QEvent>
#include <viewport.hpp>

Actor::Actor(QObject* parent) : QObject(parent), m_scale(1, 1, 1) {
	position(QVector3D(0, 0, 0));
	setParent(parent);
}

QMatrix4x4 Actor::transform() {
	QMatrix4x4 transform;
	transform.translate(position());
	transform.rotate(orientation());
	transform.scale(scale());
	return transform;
}

void Actor::setParent(QObject *parent) {
	QObject::setParent(parent);

	auto actor = static_cast<Actor*>(parent);
	if(actor != nullptr) {
		position(m_position - actor->position());
		orientation(m_orientation - actor->orientation());
		scale(m_scale - actor->scale());
	}

	m_viewport = findParent<Viewport>();
	if(m_viewport != nullptr) {
		connect(this, &Actor::moved, m_viewport, &Viewport::updateLights);
		connect(this, &Actor::rotated, m_viewport, &Viewport::updateLights);
		connect(this, &Actor::scaled, m_viewport, &Viewport::updateLights);
	}
}

bool Actor::event(QEvent* event) {
	if(!QObject::event(event)) {
		auto parent = findParent<Actor>();
		if(parent != nullptr)
			return parent->event(event);
		else
			return false;
	} else {
		return true;
	}
}

Viewport* Actor::viewport() {
	return m_viewport;
}
