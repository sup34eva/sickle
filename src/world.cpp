// Copyright 2015 PsychoLama

#include <world.hpp>
#include <sphere.hpp>
#include <cercle.hpp>
#include <light.hpp>
#include <group.hpp>
#include <viewport.hpp>

Zone::Zone(QObject *parent) : QObject(parent) {
	// NOOP
}

void Zone::addBaseProps() {
	// Ajoute le ciel
	auto sky = addChild<Sphere>();
	sky->setObjectName(tr("Sky"));
	sky->setColor(0, QColor(0, 255, 255));
	sky->scale(QVector3D(-1, -1, -1));
	sky->castShadows(false);

	// Ajoute le sol
	auto ground = addChild<Cercle>();
	ground->setObjectName(tr("Ground"));
	ground->orientation(fromEuler(QVector3D(0, 0, 90)));
	ground->castShadows(false);
	qobject_cast<Material*>(ground->material())->roughness(100);
	for(int i = 0; i < 2; i++)
		ground->setColor(i, QColor(185, 150, 110));

	// Groupe le décor
	auto group = addChild<Group>();
	group->setObjectName(tr("Scenery"));
	group->position(QVector3D(0, -1, 0));
	group->scale(QVector3D(50, 50, 50));
	sky->setParent(group);
	ground->setParent(group);

	// Ajoute la lumière du soleil
	auto light = addChild<Light>();
	light->setObjectName(tr("Sunlight"));
	light->orientation(fromEuler(QVector3D(-45, 0, -45)));
}

void Zone::draw(const DrawInfo& info) {
	// Affiche recursivement tous les enfants de la zone
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

World::World(QObject* parent) : QObject(parent), m_currentZone(0) {
	// Initialise l'ambient occlusion
	m_AO = new AmbientOcclusion(this);
}

void World::setCurrentZoneId(int zone) {
	// Change la zone en vérifiant que le nouvel ID est valide
	m_currentZone = qMax(0, qMin(zone, m_zones.size() - 1));
}

Zone* World::currentZone() {
	return m_zones.at(m_currentZone);
}

int World::addZone() {
	// Crée une zone
	auto zone = new Zone(this);

	// Ajoute le décor
	zone->addBaseProps();

	// Ajoute la zone a la liste
	m_zones.append(zone);

	// Selectionne cette nouvelle zone
	auto index = m_zones.size() - 1;
	setCurrentZoneId(index);

	// Envoie le signal zoneAdded
	zoneAdded(index);
	return index;
}
