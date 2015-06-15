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
	auto sky = addChild<Sphere>();
	sky->setObjectName(tr("Sky"));
	sky->setColor(0, QColor(0, 255, 255));
	sky->scale(QVector3D(-1, -1, -1));
	sky->castShadows(false);

	auto ground = addChild<Cercle>();
	ground->setObjectName(tr("Ground"));
	ground->orientation(fromEuler(QVector3D(0, 0, -90)));
	ground->castShadows(false);
	qobject_cast<Material*>(ground->material())->roughness(100);
	for(int i = 0; i < 2; i++)
		ground->setColor(i, QColor(185, 150, 110));

	auto group = addChild<Group>();
	group->setObjectName(tr("Scenery"));
	group->position(QVector3D(0, -1, 0));
	group->scale(QVector3D(50, 50, 50));
	sky->setParent(group);
	ground->setParent(group);

	auto light = addChild<Light>();

	light->setObjectName(tr("Sunlight"));
	light->orientation(fromEuler(QVector3D(-45, 0, -45)));
}

void Zone::draw(const DrawInfo& info) {
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

World::World(QObject* parent) : QObject(parent), m_currentZone(0) {
	m_AO = new AmbientOcclusion(this);
}

void World::setCurrentZoneId(int zone) {
	m_currentZone = qMax(0, qMin(zone, m_zones.size() - 1));
}

Zone* World::currentZone() {
	return m_zones.at(m_currentZone);
}

int World::addZone() {
	auto zone = new Zone(this);
	zone->addBaseProps();
	m_zones.append(zone);
	auto index = m_zones.size() - 1;
	setCurrentZoneId(index);
	zoneAdded(index);
	return index;
}
