// Copyright 2015 PsychoLama

#include <cylinder.hpp>
#include <QtMath>
#include <vector>

template<>
int Cylinder::tBase::s_instances = 0;

Cylinder::Cylinder(QObject* parent) : Geometry(parent) {
	setObjectName(QString(tr("Cylinder %1")).arg(Cylinder::tBase::s_instances));
	colors({
		QColor(255, 0, 0),
		QColor(0, 255, 0),
		QColor(0, 0, 255)
	});
}

template<>
QOpenGLVertexArrayObject* Cylinder::tBase::s_vao = nullptr;
template<>
ProgramList Cylinder::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Cylinder::tBase::s_buffers = {};

int sides = 30;

QVector<GLfloat> calcCylinderVertices() {
	QVector<GLfloat> vertices;
	vertices.reserve(sides* 2 * 3);

	qreal theta = 0;
	for (int j = 0; j < sides * 6; j += 6) {
		vertices.insert(j, qCos(theta));
		vertices.insert(j + 1, 1.0f);
		vertices.insert(j + 2, qSin(theta));

		vertices.insert(j + 3, qCos(theta));
		vertices.insert(j + 4, -1.0f);
		vertices.insert(j + 5, qSin(theta));

		theta += (2 * M_PI) / sides;
	}

	return vertices;
}

QVector<quint32> calcCylinderIndices() {
	QVector<quint32> indices;

	for (int j = 0; j < sides * 2; j += 2) {
		int mod = sides * 2;
		indices.append(j % mod);
		indices.append((j + 2) % mod);
		indices.append((j + 1) % mod);

		indices.append((j + 2) % mod);
		indices.append((j + 3) % mod);
		indices.append((j + 1) % mod);

		if(j > 0) {
			indices.append(j % mod);
			indices.append(0);
			indices.append((j + 2) % mod);

			indices.append((j + 3) % mod);
			indices.append(1);
			indices.append((j + 1) % mod);
		}
	}

	return indices;
}

QVector<GLfloat> calcCylinderColors() {
	QVector<GLfloat> colors;
	colors.reserve(sides);

	for (int j = 0; j < sides; j++) {
		colors.append(0.0f);
	}

	return colors;
}

QVector<GLfloat> calcCylinderUVs() {
	QVector<GLfloat> UVs;
	UVs.reserve(sides * 2);

	for (int j = 0; j < sides * 2; j++) {
		UVs.append(j % 2);
		UVs.append(static_cast<float>(j) / (sides * 2.0f));
	}

	return UVs;
}

template<>
QVector<quint32> Cylinder::tBase::s_indexBuffer = calcCylinderIndices();

template<>
QHash<QString, QVector<GLfloat>> Cylinder::tBase::s_buffersData = {
	{"Position", calcCylinderVertices()},
	{"Color", calcCylinderColors()},
	{"UV", calcCylinderUVs()}
};
