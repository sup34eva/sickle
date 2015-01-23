#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <globals.hpp>
#include <memory>

typedef struct DrawInfo {
    QMatrix4x4 View;
    QMatrix4x4 Projection;
    GLenum mode;
} DrawInfo;

/*! \class Geometry
 * \brief Base de tous les acteurs
 *
 * Cette classe est une base gerant de rendu d'un buffer de vertices.
 * Ses 3 propriétés position, orientation et scale permettent de déplacer l'objet.
 */
class Geometry : public QObject
{
    Q_OBJECT
public:
    Geometry(QObject* parent = nullptr);
    void draw(const DrawInfo &info);
    propSig(QVector3D, position, moved)
    propSig(QQuaternion, orientation, rotated)
    propSig(QVector3D, scale, scaled)
protected:
    QMatrix4x4 transform();

    /*! \brief Initialise les shaders et alloue les buffer
     *
     * Cette méthode statique est appelée par le constructeur de toutes les classes enfant de Geometry.
     * Elle n'est exécutée qu'une seule fois par classe gràce a un compteur d'instances.
     * Cela permet de n'utiliser qu'un couple shader / buffer pour l'ensemble des géometries d'une même classe.
     * Elle DOIT se trouver dans le header pour permettre au template d'être parsé corectement.
     */
    template<typename Child>
    static void initProgram(QObject *parent) {
        if(Child::s_instances++ == 0) {
            Child::s_program = new QOpenGLShaderProgram(parent);
            Child::s_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/unlit.vert");
            Child::s_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/unlit.frag");
            Child::s_program->link();

            auto posAttr = Child::s_program->attributeLocation("vertexPosition");
            auto colAttr = Child::s_program->attributeLocation("vertexColor");

            Child::s_vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            Child::s_vertexBuffer->create();
            Child::s_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
            if (!Child::s_vertexBuffer->bind())
            {
                qWarning() << "Could not bind vertex buffer to the context";
                return;
            }
            Child::s_vertexBuffer->allocate(&Child::s_vertices[0], Child::s_vertices.size() * sizeof(GLfloat));

            Child::s_colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
            Child::s_colorBuffer->create();
            Child::s_colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
            if (!Child::s_colorBuffer->bind())
            {
                qWarning() << "Could not bind color buffer to the context";
                return;
            }
            Child::s_colorBuffer->allocate(&Child::s_colors[0], Child::s_colors.size() * sizeof(GLfloat));

            Child::s_indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
            Child::s_indexBuffer->create();
            Child::s_indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
            if (!Child::s_indexBuffer->bind())
            {
                qWarning() << "Could not bind index buffer to the context";
                return;
            }
            Child::s_indexBuffer->allocate(&Child::s_indices[0], Child::s_indices.size() * sizeof(GLfloat));

            if (!Child::s_program->bind())
            {
               qWarning() << "Could not bind shader to context";
               return;
            }

            Child::s_vertexBuffer->bind();
            Child::s_program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3);
            Child::s_program->enableAttributeArray(posAttr);

            Child::s_colorBuffer->bind();
            Child::s_program->setAttributeBuffer(colAttr, GL_FLOAT, 0, 3);
            Child::s_program->enableAttributeArray(colAttr);
        }
    }
private:
    // Instances
    static int s_instances;
    static QOpenGLShaderProgram* s_program;
    static QOpenGLBuffer* s_vertexBuffer;
    static QOpenGLBuffer* s_colorBuffer;
    static QOpenGLBuffer* s_indexBuffer;
};

#endif // GEOMETRY_H
