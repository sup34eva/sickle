// Copyright 2015 PsychoLama

#include <actor.hpp>

Actor::Actor(QObject* parent) : QObject(parent), m_scale(1, 1, 1) {
}

QMatrix4x4 Actor::transform() {
	QMatrix4x4 transform;
	transform.translate(position());
	transform.rotate(orientation());
	transform.scale(scale());
	return transform;
}
