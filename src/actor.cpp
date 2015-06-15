// Copyright 2015 PsychoLama

#include <actor.hpp>
#include <QEvent>
#include <viewport.hpp>

Actor::Actor(QObject* parent) : QObject(parent), m_position(0, 0, 0), m_orientation(1, 0, 0, 0), m_scale(1, 1, 1) {
	// Permet de mettre a jour la transformation de l'acteur et d'enregistrer les evenements du viewport
	setParent(parent);
}

QMatrix4x4 Actor::transform() {
	// Crée une matrice et lui applique successivement les transformations
	QMatrix4x4 transform;
	transform.translate(position());
	transform.rotate(orientation());
	transform.scale(scale());
	return transform;
}

void Actor::setParent(QObject *parent) {
	QObject::setParent(parent);

	// Retrouve le viewport parmis l'arborescence
	m_viewport = findParent<Viewport>();
	if(m_viewport != nullptr) {
		// Met a jour les lumières de la scène lorsque cet Acteur se déplace
		connect(this, &Actor::moved, m_viewport, &Viewport::updateLights);
		connect(this, &Actor::rotated, m_viewport, &Viewport::updateLights);
		connect(this, &Actor::scaled, m_viewport, &Viewport::updateLights);
	}
}

bool Actor::event(QEvent* event) {
	// Si l'evement n'a pas été traité
	if(!QObject::event(event)) {
		// Trouv le premier parent Acteur
		auto parent = findParent<Actor>();
		if(parent != nullptr)
			return parent->event(event);  // Et lui relaie l'évenement
		else
			return false;
	} else {
		return true;
	}
}

Viewport* Actor::viewport() {
	return m_viewport;
}
