#include <geometry.h>
#include <viewport.h>

Geometry::Geometry(QObject* parent) : m_scale(1, 1, 1), m_indexBuffer(QOpenGLBuffer::IndexBuffer)
{
    m_program = new QOpenGLShaderProgram;
    m_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lit.vert");
    m_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lit.frag");
    m_program->link();
    m_posAttr = m_program->attributeLocation("vertexPosition");
    m_colAttr = m_program->attributeLocation("vertexColor");
    m_normAttr = m_program->attributeLocation("vertexNormal");

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

    GLfloat normals[6 * 4 * 3];

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

    static const unsigned int indices[] = {
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

    m_vertexBuffer.create();
    m_vertexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (!m_vertexBuffer.bind())
    {
        qWarning() << "Could not bind vertex buffer to the context";
        return;
    }
    m_vertexBuffer.allocate(vertices, sizeof(vertices));

    m_colorBuffer.create();
    m_colorBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (!m_colorBuffer.bind())
    {
        qWarning() << "Could not bind color buffer to the context";
        return;
    }
    m_colorBuffer.allocate(colors, sizeof(colors));

    m_normalBuffer.create();
    m_normalBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (!m_normalBuffer.bind())
    {
        qWarning() << "Could not bind normal buffer to the context";
        return;
    }
    m_normalBuffer.allocate(normals, sizeof(normals));

    m_indexBuffer.create();
    m_indexBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);
    if (!m_indexBuffer.bind())
    {
        qWarning() << "Could not bind index buffer to the context";
        return;
    }
    m_indexBuffer.allocate(indices, sizeof(indices));

    if (!m_program->bind())
    {
       qWarning() << "Could not bind shader program to context";
       return;
    }

    m_vertexBuffer.bind();
    m_program->setAttributeBuffer(m_posAttr, GL_FLOAT, 0, 3);
    m_program->enableAttributeArray(m_posAttr);

    m_colorBuffer.bind();
    m_program->setAttributeBuffer(m_colAttr, GL_FLOAT, 0, 3);
    m_program->enableAttributeArray(m_colAttr);

    m_normalBuffer.bind();
    m_program->setAttributeBuffer(m_normAttr, GL_FLOAT, 0, 3);
    m_program->enableAttributeArray(m_normAttr);
}

QMatrix4x4 Geometry::transform() {
    QMatrix4x4 transform;
    transform.scale(scale());
    transform.translate(position());
    transform.rotate(orientation());
    return transform;
}

void Geometry::draw(QMatrix4x4& View, QMatrix4x4& Projection)
{
    m_program->bind();

    auto Model = transform();

    m_program->setUniformValue("model", Model);
    m_program->setUniformValue("view", View);
    m_program->setUniformValue("projection", Projection);
    m_program->setUniformValue("lightPosition", QVector3D(4.5f, 4.5f, 4.0f));
    m_program->setUniformValue("lightColor", QVector3D(1.0f, 1.0f, 1.0f));
    m_program->setUniformValue("lightPower", 25.0f);
    m_program->setUniformValue("ambientColor", QVector3D(0.1f, 0.1f, 0.1f));

    auto func = QOpenGLContext::currentContext()->functions();
    func->glDrawElements(GL_TRIANGLES, 12 * 3, GL_UNSIGNED_INT, (void*)0);

    m_program->release();
}

Geometry::~Geometry()
{
    delete m_program;
}

/*QDataStream& operator<<(QDataStream& stream, const QObject* obj) {
    auto metaObject = obj->metaObject();
    for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
        stream << obj->property(metaObject->property(i).name());
    }
    return stream;
}

QDataStream& operator>>(QDataStream& stream, QObject* obj) {
    auto metaObject = obj->metaObject();
    for(int i = metaObject->propertyOffset(); i < metaObject->propertyCount(); ++i) {
        auto prop = metaObject->property(i);
        QVariant value;
        stream >> value;
        obj->setProperty(prop.name(), value);
    }
    return stream;
}*/
