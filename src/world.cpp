// Copyright 2015 PsychoLama

#include <world.hpp>

Zone::Zone(QObject *parent) : QObject(parent) {
	//
}

void Zone::draw(const DrawInfo& info) {
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(info);
	}
}

World::World(QObject* parent) : QObject(parent), m_currentZone(0) {
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
