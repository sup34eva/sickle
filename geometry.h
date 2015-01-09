#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include "globals.h"

class Geometry : public QObject
{
    Q_OBJECT
public:
    Geometry(QObject* parent = nullptr);
    ~Geometry();
    virtual void draw(QMatrix4x4& View, QMatrix4x4& Projection);
    propSig(QVector3D, position, moved)
    propSig(QQuaternion, orientation, rotated)
    propSig(QVector3D, scale, scaled)
protected:
    QMatrix4x4 transform();
private:
    //Program
    QOpenGLShaderProgram* m_program;
    QOpenGLBuffer m_vertexBuffer;
    QOpenGLBuffer m_colorBuffer;
    QOpenGLBuffer m_normalBuffer;
    QOpenGLBuffer m_indexBuffer;
    // Shader attributes
    GLuint m_posAttr;
    GLuint m_colAttr;
    GLuint m_normAttr;
    GLuint m_matrixUniform;
};

#endif // GEOMETRY_H
