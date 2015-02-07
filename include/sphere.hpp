// Copyright 2015 PsychoLama

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometry.hpp>
#include <QMetaProperty>
#include <QColor>

class Sphere : public Geometry<Sphere> {
	Q_OBJECT

public:
	explicit Sphere(QObject* parent = nullptr);
	Sphere(const Sphere& copy) : Geometry(copy.parent()) {
		auto metaObject = copy.metaObject();
		for (int i = 0; i < metaObject->propertyCount(); ++i) {
			auto prop = metaObject->property(i);
			setProperty(prop.name(), copy.property(prop.name()));
		}
	}
	prop(QVariantList, colors);
	typedef Geometry<Sphere> tBase;
};

Q_DECLARE_METATYPE(Sphere)

#endif  // SPHERE_HPP
