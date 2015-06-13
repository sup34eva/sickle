// Copyright 2015 PsychoLama

#include <sphere.hpp>
#include <QtMath>
#include <vector>

template<>
int Sphere::tBase::s_instances = 0;

Sphere::Sphere(QObject* parent) : Geometry(parent) {
	setObjectName(QString(tr("Sphere %1")).arg(Sphere::tBase::s_instances));
	colors({
		QColor(255, 0, 0)
	});
}

template<>
QOpenGLVertexArrayObject* Sphere::tBase::s_vao = nullptr;
template<>
ProgramList Sphere::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Sphere::tBase::s_buffers = {};

const quint32 rings = 24;
const quint32 sectors = 48;

QVector<GLfloat> calcVertices() {
	const auto R = 1. / (qreal)(rings - 1);
	const auto S = 1. / (qreal)(sectors - 1);
	QVector<GLfloat> vertices;
	vertices.reserve(rings * sectors * 3);

	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		vertices.append(qCos(2 * M_PI * s * S) * qSin(M_PI * r * R));
		vertices.append(qSin(-M_PI_2 + M_PI * r * R));
		vertices.append(qSin(2 * M_PI * s * S) * qSin(M_PI * r * R));
	}
	return vertices;
}

QVector<quint32> calcIndices() {
	QVector<quint32> indices;
	auto size = rings * sectors * 3;
	indices.reserve(size * 2);

	for (quint32 r = 0; r < rings - 1; r++) for (quint32 s = 0; s < sectors - 1; s++) {
		auto currRow = r * sectors;
		auto nextRow = (r + 1) * sectors;

		indices.append(nextRow + s);
		indices.append(currRow + (s + 1));
		indices.append(currRow + s);

		indices.append(nextRow + (s + 1));
		indices.append(currRow + (s + 1));
		indices.append(nextRow + s);
	}
	return indices;
}

QVector<GLfloat> calcColors() {
	QVector<GLfloat> colors;
	colors.reserve(rings * sectors);

	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		colors.append(0.0f);
	}
	return colors;
}

QVector<GLfloat> calcUVs() {
	const auto R = 1. / (qreal)(rings - 1);
	const auto S = 1. / (qreal)(sectors - 1);
	QVector<GLfloat> UVs;
	UVs.reserve(rings * sectors * 2);

	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		UVs.append(s * S);
		UVs.append(r * R);
	}
	return UVs;
}

template<>
QVector<quint32> Sphere::tBase::s_indexBuffer = calcIndices();

template<>
QHash<QString, QVector<GLfloat>> Sphere::tBase::s_buffersData = {
	{"Position", calcVertices()},
	{"Color", calcColors()},
	{"UV", calcUVs()}
};
