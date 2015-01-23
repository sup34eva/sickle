#include <geometry.h>
#include <viewport.h>

int Geometry::s_instances = 0;
QOpenGLShaderProgram* Geometry::s_program = nullptr;
QOpenGLBuffer* Geometry::s_vertexBuffer = nullptr;
QOpenGLBuffer* Geometry::s_colorBuffer = nullptr;
//QOpenGLBuffer* Geometry::s_normalBuffer = nullptr;
QOpenGLBuffer* Geometry::s_indexBuffer = nullptr;

void Geometry::initProgram(Geometry* self, QObject* parent) {
    if(s_instances++ == 0) {
        s_program = new QOpenGLShaderProgram(parent);
        s_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/unlit.vert");
        s_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/unlit.frag");
        s_program->link();

        auto posAttr = s_program->attributeLocation("vertexPosition");
        auto colAttr = s_program->attributeLocation("vertexColor");
        //auto normAttr = s_program->attributeLocation("vertexNormal");

        /*s_normals.resize(s_vertices.size());
        for(auto i = 0; i < s_indices.size(); i += 3) {
            QVector3D p1 = QVector3D(s_vertices[s_indices[i]], s_vertices[s_indices[i] + 1], s_vertices[s_indices[i] + 2]),
                      p2 = QVector3D(s_vertices[s_indices[i + 1]], s_vertices[s_indices[i + 1] + 1], s_vertices[s_indices[i + 1] + 2]),
                      p3 = QVector3D(s_vertices[s_indices[i + 2]], s_vertices[s_indices[i + 2] + 1], s_vertices[s_indices[i + 2] + 2]),
                      U1 = p2 - p1,
                      V1 = p3 - p1,
                      Normal = QVector3D::crossProduct(U1, V1);
            Normal.normalize();
            s_normals[s_indices[i] + 0] = s_normals[s_indices[i + 1] + 0] = s_normals[s_indices[i + 2] + 0] = Normal.x();
            s_normals[s_indices[i] + 1] = s_normals[s_indices[i + 1] + 1] = s_normals[s_indices[i + 2] + 1] = Normal.y();
            s_normals[s_indices[i] + 2] = s_normals[s_indices[i + 1] + 2] = s_normals[s_indices[i + 2] + 2] = Normal.z();
        }*/

        s_vertexBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_vertexBuffer->create();
        s_vertexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_vertexBuffer->bind())
        {
            qWarning() << "Could not bind vertex buffer to the context";
            return;
        }
        auto vertices = self->getVertices();
        s_vertexBuffer->allocate(&vertices[0], vertices.size() * sizeof(GLfloat));

        s_colorBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_colorBuffer->create();
        s_colorBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_colorBuffer->bind())
        {
            qWarning() << "Could not bind color buffer to the context";
            return;
        }
        auto colors = self->getColors();
        s_colorBuffer->allocate(&colors[0], colors.size() * sizeof(GLfloat));

        /*s_normalBuffer = new QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
        s_normalBuffer->create();
        s_normalBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_normalBuffer->bind())
        {
            qWarning() << "Could not bind normal buffer to the context";
            return;
        }
        s_normalBuffer->allocate(&s_normals[0], s_normals.size() * sizeof(GLfloat));*/

        s_indexBuffer = new QOpenGLBuffer(QOpenGLBuffer::IndexBuffer);
        s_indexBuffer->create();
        s_indexBuffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
        if (!s_indexBuffer->bind())
        {
            qWarning() << "Could not bind index buffer to the context";
            return;
        }
        auto indices = self->getIndices();
        s_indexBuffer->allocate(&indices[0], indices.size() * sizeof(GLfloat));

        if (!s_program->bind())
        {
           qWarning() << "Could not bind shader to context";
           return;
        }

        s_vertexBuffer->bind();
        s_program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(posAttr);

        s_colorBuffer->bind();
        s_program->setAttributeBuffer(colAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(colAttr);

        /*s_normalBuffer->bind();
        s_program->setAttributeBuffer(normAttr, GL_FLOAT, 0, 3);
        s_program->enableAttributeArray(normAttr);*/
    }
}

Geometry::Geometry(QObject* parent) : QObject(parent), m_scale(1, 1, 1)
{
    //initProgram(this, parent);
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

    auto MVP = info.Projection * info.View * transform();
    s_program->setUniformValue("MVP", MVP);

    auto func = QOpenGLContext::currentContext()->functions();
    func->glDrawElements(info.mode, 12 * 3, GL_UNSIGNED_INT, 0);

    s_program->release();
}
