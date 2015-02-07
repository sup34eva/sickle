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
QOpenGLBuffer* Sphere::tBase::s_indexBuffer = nullptr;

std::vector<GLfloat> calcVertices(quint32 rings, quint32 sectors) {
	const auto R = 1. / (qreal)(rings - 1);
	const auto S = 1. / (qreal)(sectors - 1);
	std::vector<GLfloat> vertices;
	vertices.resize(rings * sectors * 3);

	auto v = vertices.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		float const y = qSin(-M_PI_2 + M_PI * r * R);
		float const x = qCos(2 * M_PI * s * S) * qSin(M_PI * r * R);
		float const z = qSin(2 * M_PI * s * S) * qSin(M_PI * r * R);
		*v++ = x;
		*v++ = y;
		*v++ = z;
	}

	return vertices;
}

std::vector<quint32> calcIndices(quint32 rings, quint32 sectors) {
	std::vector<quint32> indices;
	indices.resize(rings * sectors * 6);
	auto i = indices.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		auto currRow = r * sectors;
		auto nextRow = (r+1) * sectors;

		*i++ = nextRow + s;
		*i++ = currRow + (s + 1);
		*i++ = currRow + s;

		*i++ = nextRow + (s + 1);
		*i++ = currRow + (s + 1);
		*i++ = nextRow + s;
	}
	return indices;
}

std::vector<GLfloat> calcColors(quint32 rings, quint32 sectors) {
	std::vector<GLfloat> colors;
	colors.resize(rings * sectors * 3);
	auto c = colors.begin();
	for (quint32 r = 0; r < rings; r++) for (quint32 s = 0; s < sectors; s++) {
		*c++ = 1.0f;
		*c++ = 0.0f;
		*c++ = 0.0f;
	}
	return colors;
}

template<>
std::vector<GLfloat> Sphere::tBase::s_vertices = calcVertices(12, 24);
template<>
std::vector<quint32> Sphere::tBase::s_indices = calcIndices(12, 24);
template<>
std::vector<GLfloat> Sphere::tBase::s_colors = calcColors(12, 24);
