#include <geometry.h>
#include <viewport.h>

int Geometry::s_instances = 0;
QOpenGLShaderProgram* Geometry::s_program = nullptr;
QOpenGLBuffer* Geometry::s_vertexBuffer = nullptr;
QOpenGLBuffer* Geometry::s_colorBuffer = nullptr;
QOpenGLBuffer* Geometry::s_normalBuffer = nullptr;
QOpenGLBuffer* Geometry::s_indexBuffer = nullptr;

void Geometry::initProgram(QObject* parent) {
    auto index = s_instances++;

    if(index == 0) {
        s_program = new QOpenGLShaderProgram(parent);
        s_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/unlit.vert");
        //m_s_program->addShaderFromSourceFile(QOpenGLShader::Geometry, ":/shaders/unlit.geom");
        s_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/unlit.frag");
        s_program->link();

        GLuint posAttr = s_program->attributeLocation("vertexPosition");
        GLuint colAttr = s_program->attributeLocation("vertexColor");
        GLuint normAttr = s_program->attributeLocation("vertexNormal");

        GLfloat vertices[] = {
            -1.0f, -1.0f, -1.0f, // Face 1
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f, // Face 2
             1.0f,  1.0f, -1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f, -1.0f, // Face 3
             1.0f, -1.0f, -1.0f,
             1.0f, -1.0f,  1.0f,
            -1.0f, -1.0f,  1.0f,
            -1.0f,  1.0f, -1.0f, // Face 4
            -1.0f,  1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
             1.0f,  1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f, // Face 5
            -1.0f,  1.0f, -1.0f,
             1.0f,  1.0f, -1.0f,
             1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f,  1.0f, // Face 6
             1.0f, -1.0f,  1.0f,
             1.0f,  1.0f,  1.0f,
            -1.0f,  1.0f,  1.0f,
        };

        quint32 indices[] = {
            0, 1, 3,
            3, 1, 2,
            4, 5, 7,
            7, 5, 6,
            8, 9, 11,
            11, 9, 10,
            12, 13, 15,
            15, 13, 14,
            16, 17, 19,
            19, 17, 18,
            20, 21, 23,
            23, 21, 22,
        };

        GLfloat colors[] = {
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
        };

        auto normals = new GLfloat[6 * 4 * 3];
        for(int i = 0; i < (6 * 4 * 3); i += 12) {
            QVector3D p1 = QVector3D(vertices[i + 0], vertices[i + 1], vertices[i + 2]),
                      p2 = QVector3D(vertices[i + 3], vertices[i + 4], vertices[i + 5]),
                      p3 = QVector3D(vertices[i + 6], vertices[i + 7], vertices[i + 8]),
                      p4 = QVector3D(vertices[i + 9], vertices[i + 10], vertices[i + 11]),
                      U1 = p2 - p1,
                      V1 = p3 - p1,
                      U2 = p2 - p4,
                      V2 = p3 - p4,
                      Normal1 = QVector3D::crossProduct(U1, V1),
                      Normal2 = QVector3D::crossProduct(U2, V2),
                      Normal;
            Normal1.normalize();
            Normal2.normalize();
            Normal = Normal1 + Normal2;
            Normal.normalize();
            normals[i + 0] = normals[i + 3] = normals[i + 6] = normals[i + 9] = Normal.x();
            normals[i + 1] = normals[i + 4] = normals[i + 7] = normals[i + 10] = Normal.y();
            normals[i + 2] = normals[i + 5] = normals[i + 8] = normals[i + 11] = Normal.z();
        }

        s_vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_vertexBuffer->create();
        s_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_vertexBuffer->bind())
        {
            qWarning() << "Could not bind vertex buffer to the context";
            return;
        }
        s_vertexBuffer->allocate(vertices, sizeof(vertices));

        s_colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_colorBuffer->create();
        s_colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_colorBuffer->bind())
        {
            qWarning() << "Could not bind color buffer to the context";
            return;
        }
        s_colorBuffer->allocate(colors, sizeof(colors));

        s_normalBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_normalBuffer->create();
        s_normalBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_normalBuffer->bind())
        {
            qWarning() << "Could not bind normal buffer to the context";
            return;
        }
        s_normalBuffer->allocate(normals, sizeof(normals));

        s_indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        s_indexBuffer->create();
        s_indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_indexBuffer->bind())
        {
            qWarning() << "Could not bind index buffer to the context";
            return;
        }
        s_indexBuffer->allocate(indices, sizeof(indices));

        if (!s_program->bind())
        {
           qWarning() << "Could not bind shader s_program to context";
           return;
        }

        s_vertexBuffer->bind();
        s_program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(posAttr);

        s_colorBuffer->bind();
        s_program->setAttributeBuffer(colAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(colAttr);

        s_normalBuffer->bind();
        s_program->setAttributeBuffer(normAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(normAttr);
    }
}

Geometry::Geometry(QObject* parent) : QObject(parent), m_scale(1, 1, 1)
{
    initProgram(parent);
}

QMatrix4x4 Geometry::transform() {
    QMatrix4x4 transform;
    transform.translate(position());
    transform.rotate(orientation());
    transform.scale(scale());
    return transform;
}

void Geometry::draw(const DrawInfo &info)
{
    s_program->bind();

    s_program->setUniformValue("model", transform());
    s_program->setUniformValue("view", info.View);
    s_program->setUniformValue("projection", info.Projection);
    s_program->setUniformValue("lightPosition", QVector3D(4.5f, 4.5f, 4.0f));
    s_program->setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    s_program->setUniformValue("lightPower", 25.0f);
    s_program->setUniformValue("ambientColor", QVector3D(0.1f, 0.1f, 0.1f));

    auto func = QOpenGLContext::currentContext()->functions();
    func->glDrawElements(info.mode, 12 * 3, GL_UNSIGNED_INT, 0);

    s_program->release();
}
