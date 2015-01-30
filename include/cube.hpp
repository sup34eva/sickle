// Copyright 2015 PsychoLama

#ifndef CUBE_H
#define CUBE_H

#include <geometry.hpp>
#include <globals.hpp>
#include <QColor>
#include <QMetaProperty>
#include <vector>

class Cube : public Geometry {
	Q_OBJECT

public:
	explicit Cube(QObject* parent = nullptr);
	Cube(const Cube& copy) :Geometry(copy.parent()) {
		initProgram<Cube>(copy.parent());
		auto metaObject = copy.metaObject();
		for (int i = 0; i < metaObject->propertyCount(); ++i) {
			auto prop = metaObject->property(i);
			setProperty(prop.name(), copy.property(prop.name()));
		}
	}
	prop(QVariantList, colors);
	static std::vector<GLfloat> s_vertices;
	static std::vector<GLfloat> s_colors;
	static std::vector<quint32> s_indices;
};

Q_DECLARE_METATYPE(Cube)

#endif  // CUBE_H
