// Copyright 2015 PsychoLama

#include <sphere.hpp>
#include <QtMath>
#include <vector>

template<>
int Sphere::tBase::s_instances = 0;

Sphere::Sphere(QObject* parent) : Geometry(parent) {
	setObjectName(QString(tr("Sphere %1")).arg(Sphere::tBase::s_instances));
	colors({
		QColor(1, 0, 0)
	});
}

template<>
QOpenGLVertexArrayObject* Sphere::tBase::s_vao = nullptr;
template<>
QOpenGLShaderProgram* Sphere::tBase::s_program = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_vertexBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_colorBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_normalBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_UVBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_tangentBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_bitangentBuffer = nullptr;
template<>
QOpenGLBuffer* Sphere::tBase::s_indexBuffer = nullptr;

template<>
QVector<GLfloat> Sphere::tBase::s_normals = {};
template<>
QVector<GLfloat> Sphere::tBase::s_tangents = {};
template<>
QVector<GLfloat> Sphere::tBase::s_bitangents = {};

const quint32 rings = 24;
const quint32 sectors = 48;

QVector<GLfloat> calcVertices() {
	const auto R = 1. / (qreal)(rings - 1);
	const auto S = 1. / (qreal)(sectors - 1);
	QVector<GLfloat> vertices;
	vertices.resize(rings * sectors * 3);

	auto v = vertices.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		*v++ = qCos(2 * M_PI * s * S) * qSin(M_PI * r * R);
		*v++ = qSin(-M_PI_2 + M_PI * r * R);
		*v++ = qSin(2 * M_PI * s * S) * qSin(M_PI * r * R);
	}
	return vertices;
}

QVector<quint32> calcIndices() {
	QVector<quint32> indices;
	auto size = rings * sectors * 3;
	indices.resize(size * 2);
	auto i = indices.begin();
	for (quint32 r = 0; r < rings - 1; r++) for (quint32 s = 0; s < sectors - 1; s++) {
		auto currRow = r * sectors;
		auto nextRow = (r + 1) * sectors;

		*i++ = nextRow + s;
		*i++ = currRow + (s + 1);
		*i++ = currRow + s;

		*i++ = nextRow + (s + 1);
		*i++ = currRow + (s + 1);
		*i++ = nextRow + s;
	}
	return indices;
}

QVector<GLfloat> calcColors() {
	QVector<GLfloat> colors;
	colors.resize(rings * sectors * 3);
	auto c = colors.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		*c++ = 1.0f;
		*c++ = 0.0f;
		*c++ = 0.0f;
	}
	return colors;
}

QVector<GLfloat> calcUVs() {
	const auto R = 1. / (qreal)(rings - 1);
	const auto S = 1. / (qreal)(sectors - 1);
	QVector<GLfloat> UVs;
	UVs.resize(rings * sectors * 2);

	auto t = UVs.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		*t++ = s * S;
		*t++ = r * R;
	}
	return UVs;
}

template<>
QVector<GLfloat> Sphere::tBase::s_vertices = calcVertices();
template<>
QVector<quint32> Sphere::tBase::s_indices = calcIndices();
template<>
QVector<GLfloat> Sphere::tBase::s_colors = calcColors();
template<>
QVector<GLfloat> Sphere::tBase::s_uv = calcUVs();
