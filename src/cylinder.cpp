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
	vertices.reserve(sides* 4 * 3);

	qreal theta = 0;
	for (int j = 0; j < sides; j++) {
		for(int i = 0; i < 2; i++) {
			vertices.append(qCos(theta));
			vertices.append(1.0f);
			vertices.append(qSin(theta));

			vertices.append(qCos(theta));
			vertices.append(-1.0f);
			vertices.append(qSin(theta));
		}

		theta += (2 * M_PI) / sides;
	}

	return vertices;
}

QVector<quint32> calcCylinderIndices() {
	QVector<quint32> indices;

	for (int j = 0; j < sides * 4; j += 4) {
		int mod = sides * 4;
		indices.append((j + 0) % mod);
		indices.append((j + 5) % mod);
		indices.append((j + 1) % mod);

		indices.append((j + 0) % mod);
		indices.append((j + 4) % mod);
		indices.append((j + 5) % mod);

		if(j > 0) {
			indices.append((j + 2) % mod);
			indices.append(2);
			indices.append((j + 6) % mod);

			indices.append((j + 7) % mod);
			indices.append(3);
			indices.append((j + 3) % mod);
		}
	}

	return indices;
}

QVector<GLfloat> calcCylinderColors() {
	QVector<GLfloat> colors;
	colors.reserve(sides * 4);

	for (int j = 0; j < sides * 4; j++) {
		colors.append(qMax(0, (j % 4) - 1));
	}

	return colors;
}

QVector<GLfloat> calcCylinderUVs() {
	QVector<GLfloat> UVs;
	UVs.reserve(sides * 4 * 2);

	qreal theta = 0;
	for (int j = 0; j < sides; j++) {
		for(int i = 0; i < 2; i++) {
			UVs.append(i);
			UVs.append(static_cast<float>(j) / sides);
		}

		UVs.append(qCos(theta));
		UVs.append(qSin(theta));

		UVs.append(qCos(theta));
		UVs.append(qSin(theta));

		theta += (2 * M_PI) / sides;
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
