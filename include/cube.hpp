// Copyright 2015 PsychoLama

#ifndef CUBE_H
#define CUBE_H

#include <geometry.hpp>
#include <globals.hpp>
#include <QColor>
#include <vector>

class Cube : public Geometry {
	Q_OBJECT
	Q_CLASSINFO("Type", "Cube")

public:
	explicit Cube(QObject* parent = nullptr);
	prop(QVariantList, colors);
	static std::vector<GLfloat> s_vertices;
	static std::vector<GLfloat> s_colors;
	static std::vector<quint32> s_indices;
};

#endif  // CUBE_H
