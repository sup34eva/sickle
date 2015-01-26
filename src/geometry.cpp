#include <geometry.hpp>
#include <viewport.hpp>

int Geometry::s_instances = 0;
QOpenGLShaderProgram* Geometry::s_program = nullptr;
QOpenGLBuffer* Geometry::s_vertexBuffer = nullptr;
QOpenGLBuffer* Geometry::s_colorBuffer = nullptr;
QOpenGLBuffer* Geometry::s_indexBuffer = nullptr;

Geometry::Geometry(QObject* parent) : QObject(parent), m_scale(1, 1, 1) {
}

QMatrix4x4 Geometry::transform() {
	QMatrix4x4 transform;
	transform.translate(position());
	transform.rotate(orientation());
	transform.scale(scale());
	return transform;
}

void Geometry::draw(const DrawInfo& info) {
	s_program->bind();

	auto MVP = info.Projection * info.View * transform();
	s_program->setUniformValue("MVP", MVP);

	auto func = QOpenGLContext::currentContext()->functions();
	func->glDrawElements(info.mode, s_indexBuffer->size() / sizeof(GLfloat), GL_UNSIGNED_INT, 0);

	s_program->release();
}
