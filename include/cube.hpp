#ifndef CUBE_H
#define CUBE_H

#include <geometry.hpp>
#include <globals.hpp>

class Cube : public Geometry {
	Q_OBJECT
public:
	Cube(QObject* parent = nullptr);
	static std::vector<GLfloat> s_vertices;
	static std::vector<GLfloat> s_colors;
	static std::vector<quint32> s_indices;
};

#endif  // CUBE_H
