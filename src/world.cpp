// Copyright 2015 PsychoLama

#include <world.hpp>
#include <sphere.hpp>
#include <cube.hpp>
#include <light.hpp>

Zone::Zone(QObject *parent) : QObject(parent) {
	auto sky = addChild<Sphere>();
	sky->setObjectName(tr("Sky"));
	sky->setColor(0, QColor(0, 255, 255));
	sky->position(QVector3D(0, -2, 0));
	sky->scale(QVector3D(-50, -50, -50));
	sky->castShadows(false);

	auto ground = addChild<Cube>();
	ground->setObjectName(tr("Ground"));
	ground->position(QVector3D(0, -2, 0));
	ground->scale(QVector3D(50, 1, 50));
	for(int i = 0; i < 6; i++)
		ground->setColor(i, QColor(185, 150, 110));

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

void World::setCurrentZone(int zone) {
	m_currentZone = zone;
}

Zone* World::currentZone() {
	if(m_currentZone >= 0 && m_currentZone < m_zones.size()) {
		return m_zones.at(m_currentZone);
	} else {
		qFatal("OOB: %d", m_currentZone);
	}
}

int World::addZone() {
	m_zones.append(new Zone(this));
	auto index = m_zones.size() - 1;
	zoneAdded(index);
	return index;
}