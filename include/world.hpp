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
		void addBaseProps();
		void draw(const DrawInfo& info);

		template<typename T>
		T* addChild() {
			return new T(this);
		}
};

struct AmbientOcclusion : public QObject {
	Q_OBJECT
public:
	explicit AmbientOcclusion(QObject* parent = nullptr) : QObject(parent) {
		kernelSize(10);
		maxDist(0.5);
		threshold(5);
	}
	prop(float, kernelSize);
	prop(float, maxDist);
	prop(float, threshold);
};

class World : public QObject {
	Q_OBJECT

	public:
		explicit World(QObject* parent = nullptr);
		Zone* currentZone();

		void setCurrentZoneId(int zone);
		int currentZoneId() {
			return m_currentZone;
		}

		QList<Zone*> zoneList() {
			return m_zones;
		}

		void setZoneList(const QList<Zone*>& list) {
			foreach(auto zone, m_zones)
				zone->deleteLater();
			m_zones = list;
		}

		prop(QObject*, AO);

	signals:
		void zoneAdded(int index);

	public slots:
		int addZone();
		void removeZone(int index) {
			m_zones.removeAt(index);
		}

	private:
		QList<Zone*> m_zones;
		int m_currentZone;
};

#endif  // WORLD_HPP
