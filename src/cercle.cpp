#include "cercle.hpp"
#include <QtMath>
#include <vector>

template<>
int Cercle::tBase::s_instances = 0;

Cercle::Cercle(QObject* parent) : Geometry(parent) {
    setObjectName(QString(tr("Cercle %1")).arg(Cercle::tBase::s_instances));
    colors({
        QColor(255, 0, 0),
        QColor(0, 255, 0),
        QColor(0, 0, 255)
    });
}

template<>
QOpenGLVertexArrayObject* Cercle::tBase::s_vao = nullptr;
template<>
ProgramList Cercle::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Cercle::tBase::s_buffers = {};

int side = 30;

QVector<GLfloat> calcCercleVertices() {
    QVector<GLfloat> vertices;
    vertices.reserve(side* 4 * 3);

    qreal theta = 0;
    for (int j = 0; j < side; j++) {

            vertices.append(qCos(theta));
            vertices.append(1.0f);
            vertices.append(qSin(theta));




        theta += (2 * M_PI) / side;
    }

    return vertices;
}

QVector<quint32> calcCercleIndices() {
    QVector<quint32> indices;

    for (int j = 0; j < side ; j++) {
        int mod = side  ;
        if(j > 0) {
            indices.append((j) % mod);
            indices.append(0);
            indices.append((j + 1) % mod);


        }
    }

    return indices;
}

QVector<GLfloat> calcCercleColors() {
    QVector<GLfloat> colors;
    colors.reserve(side  );

    for (int j = 0; j < side  ; j++) {
        colors.append(0.0f);
    }

    return colors;
}

QVector<GLfloat> calcCercleUVs() {
    QVector<GLfloat> UVs;
    UVs.reserve(side * 2);

    qreal theta = 0;
    for (int j = 0; j < side; j++) {



        UVs.append(qCos(theta));
        UVs.append(qSin(theta));



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
