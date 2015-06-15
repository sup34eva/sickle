// Copyright 2015 PsychoLama

#include "cercle.hpp"
#include <QtMath>
#include <vector>

template<>
int Cercle::tBase::s_instances = 0;

Cercle::Cercle(QObject* parent) : Geometry(parent) {
    setObjectName(QString(tr("Cercle %1")).arg(Cercle::tBase::s_instances));
    colors({
        QColor(255, 0, 0),
		QColor(0, 255, 0)
    });
}

template<>
QOpenGLVertexArrayObject* Cercle::tBase::s_vao = nullptr;
template<>
ProgramList Cercle::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Cercle::tBase::s_buffers = {};

int side = 48;

QVector<GLfloat> calcCercleVertices() {
    QVector<GLfloat> vertices;
	vertices.reserve(side * 2 * 3);

    qreal theta = 0;
    for (int j = 0; j < side; j++) {
		for (int i = 0; i < 2; i++) {
			vertices.append(qCos(theta));
			vertices.append(qSin(theta));
			vertices.append(0.0f);
		}

        theta += (2 * M_PI) / side;
    }

    return vertices;
}

QVector<quint32> calcCercleIndices() {
    QVector<quint32> indices;

	for (int j = 0; j < side * 2; j += 2) {
		int mod = side * 2;
		if(j > 0) {
			indices.append((j + 2) % mod);
			indices.append(0);
			indices.append(j % mod);

			indices.append((j + 1) % mod);
			indices.append(1);
			indices.append((j + 3) % mod);
		}
	}

    return indices;
}

QVector<GLfloat> calcCercleColors() {
    QVector<GLfloat> colors;
	colors.reserve(side * 2);

	for (int j = 0; j < side * 2; j++) {
		colors.append(j % 2);
    }

    return colors;
}

QVector<GLfloat> calcCercleUVs() {
    QVector<GLfloat> UVs;
	UVs.reserve(side * 2 * 2);

    qreal theta = 0;
	for (int j = 0; j < side; j++) {
		for (int i = 0; i < 2; i++) {
			UVs.append(qCos(theta));
			UVs.append(qSin(theta));
		}

        theta += (2 * M_PI) / side;
    }

    return UVs;
}

template<>
QVector<quint32> Cercle::tBase::s_indexBuffer = calcCercleIndices();

template<>
QHash<QString, QVector<GLfloat>> Cercle::tBase::s_buffersData = {
    {"Position", calcCercleVertices()},
    {"Color", calcCercleColors()},
    {"UV", calcCercleUVs()}
};
