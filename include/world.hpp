// Copyright 2015 PsychoLama

#ifndef WORLD_HPP
#define WORLD_HPP

#include <globals.hpp>
#include <actor.hpp>
#include <QObject>
#include <QList>

class Zone : public QObject {
	Q_OBJECT

	public:
		explicit Zone(QObject* parent = nullptr);
		void draw(const DrawInfo& info);

		template<typename T>
		T* addChild() {
			return new T(this);
		}
};

class World : public QObject {
	Q_OBJECT

	public:
		explicit World(QObject* parent = nullptr);
		Zone* currentZone();
		void setCurrentZone(int zone);

	signals:
		void zoneAdded(int index);

	public slots:
		int addZone();

	private:
		QList<Zone*> m_zones;
		int m_currentZone;
};

#endif // WORLD_HPP
