// Copyright 2015 PsychoLama

#include <pyramide.hpp>
#include <QColor>

template<>
int Pyramide::tBase::s_instances = 0;

Pyramide::Pyramide(QObject *parent) : Geometry(parent) {
    setObjectName(QString(tr("Pyramide %1")).arg(Pyramide::tBase::s_instances));
    colors({
        QColor(1, 0, 0),
        QColor(1, 1, 0),
        QColor(0, 1, 0),
        QColor(0, 1, 1),
        QColor(0, 0, 1),
    });
}

template<>
QOpenGLVertexArrayObject* Pyramide::tBase::s_vao = nullptr;
template<>
QOpenGLShaderProgram* Pyramide::tBase::s_program = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_vertexBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_colorBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_normalBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_UVBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_tangentBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_bitangentBuffer = nullptr;
template<>
QOpenGLBuffer* Pyramide::tBase::s_indexBuffer = nullptr;

template<>
QVector<GLfloat> Pyramide::tBase::s_normals = {};
template<>
QVector<GLfloat> Pyramide::tBase::s_tangents = {};
template<>
QVector<GLfloat> Pyramide::tBase::s_bitangents = {};

template<>
QVector<GLfloat> Pyramide::tBase::s_vertices = {
         0.0f,  1.0f,  0.0f,		// Face 1
        -1.0f, -1.0f,  1.0f,		//
         1.0f, -1.0f,  1.0f,		//
         0.0f,  1.0f,  0.0f,		// Face 2
         1.0f, -1.0f,  1.0f,		//
         1.0f, -1.0f, -1.0f,		//
         0.0f,  1.0f,  0.0f,		// Face 3
        -1.0f, -1.0f,  1.0f,		//
        -1.0f, -1.0f, -1.0f,		//
         0.0f,  1.0f,  0.0f,		// Face 4
        -1.0f, -1.0f, -1.0f,		//
         1.0f, -1.0f, -1.0f,		//
        -1.0f, -1.0f, -1.0f,		// Face 5
         1.0f, -1.0f, -1.0f,		//
         1.0f, -1.0f,  1.0f,		//
        -1.0f, -1.0f,  1.0f,            //



};

template<>
QVector<quint32> Pyramide::tBase::s_indices = {
        0,  1,  2,		// Face 1
        3,  4,  5,		// Face 2
        8,  7,  6,		// Face 3
        11, 10, 9,		// Face 4
        12, 13, 14,		// Face 5
        12, 14, 15,		//
};

template<>
QVector<GLfloat> Pyramide::tBase::s_colors = {
        1.0f, 0.0f, 0.0f,		// Face 1
        1.0f, 0.0f, 0.0f,		//
        1.0f, 0.0f, 0.0f,		//
        1.0f, 1.0f, 0.0f,		// Face 2
        1.0f, 1.0f, 0.0f,		//
        1.0f, 1.0f, 0.0f,		//
        0.0f, 1.0f, 0.0f,		// Face 3
        0.0f, 1.0f, 0.0f,		//
        0.0f, 1.0f, 0.0f,		//
        0.0f, 1.0f, 1.0f,		// Face 4
        0.0f, 1.0f, 1.0f,		//
        0.0f, 1.0f, 1.0f,		//
        0.0f, 0.0f, 1.0f,		// Face 5
        0.0f, 0.0f, 1.0f,		//
        0.0f, 0.0f, 1.0f,		//
        0.0f, 0.0f, 1.0f,		//

};

template<>
QVector<GLfloat> Pyramide::tBase::s_uv = {
         0.0f,  0.5f,		// Face 1
         1.0f,  0.0f,		//
         1.0f,  1.0f,		//
         0.0f,  0.5f,		// Face 2
         1.0f,  0.0f,		//
         1.0f,  1.0f,		//
         0.0f,  0.5f,		// Face 3
         1.0f,  0.0f,		//
         1.0f,  1.0f,		//
         0.0f,  0.5f,		// Face 4
         1.0f,  0.0f,		//
         1.0f,  1.0f,		//
         0.0f,  0.0f,		// Face 5
         0.0f,  1.0f,		//
         1.0f,  1.0f,		//
         1.0f,  0.0f,		//

};
