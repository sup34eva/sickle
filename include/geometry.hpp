// Copyright 2015 PsychoLama

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <globals.hpp>
#include <actor.hpp>
#include <memory>
#include <vector>

/*! \brief Base de toutes les géometries
 * \tparam Child La classe heritant de Geometry (utilisé pour initialiser les membres statiques)
 *
 * Cette classe est une base gerant de rendu d'un buffer de vertices.
 * Elle dispose de 3 buffers (s_vertices, s_colors et s_indices) devant être remplis par les classes enfant pour
 * afficher un objet.
 *
 */
template <typename Child>
class Geometry : public Actor {
public:
	explicit Geometry(QObject* parent = nullptr) : Actor(parent) {
		initProgram(parent);
	}
	noinline void draw(const DrawInfo& info) {
		Child::s_program->bind();
		Child::s_vao->bind();

		auto MVP = info.Projection * info.View * transform();
		Child::s_program->setUniformValue("MVP", MVP);

		auto func = info.context->functions();
		func->glDrawElements(info.mode, Child::s_indexBuffer->size() / sizeof(GLfloat), GL_UNSIGNED_INT, 0);

		Child::s_program->release();
	}

protected:
	template <typename T>
	noinline static QOpenGLBuffer* initBuffer(QOpenGLBuffer::Type type, const std::vector<T>& data) {
		auto buffer = new QOpenGLBuffer(type);
		buffer->create();
		buffer->setUsagePattern(QOpenGLBuffer::StaticDraw);
		if (!buffer->bind()) {
			qCritical("Could not bind buffer to the context");
			return nullptr;
		}
		buffer->allocate(&data[0], data.size() * sizeof(T));
		return buffer;
	}

	/*! \brief Initialise les shaders et alloue les buffer
	 *
	 * Cette méthode statique est appelée par le constructeur de toutes les classes enfant de Geometry.
	 * Elle n'est exécutée qu'une seule fois par classe gràce a un compteur d'instances.
	 * Cela permet de n'utiliser qu'un couple shader / buffer pour l'ensemble des géometries d'une même classe.
	 * Elle DOIT se trouver dans le header pour permettre au template d'être parsé corectement.
	 */
	noinline static void initProgram(QObject* parent) {
		if (Child::s_instances++ == 0) {
			Child::s_program = new QOpenGLShaderProgram(parent);

			Child::s_vao = new QOpenGLVertexArrayObject(parent);
			Child::s_vao->create();
			Child::s_vao->bind();

			if (!Child::s_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/unlit.vert")) {
				qCritical() << "Could not load vertex shader:" << Child::s_program->log();
				return;
			}

			if (!Child::s_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/unlit.frag")) {
				qCritical() << "Could not load fragment shader:" << Child::s_program->log();
				return;
			}

			if (!Child::s_program->link()) {
				qCritical() << "Could not link shaders:" << Child::s_program->log();
				return;
			}

			if (!Child::s_program->bind()) {
				qCritical() << "Could not bind shader to context";
				return;
			}

			Child::s_vertexBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_vertices);
			Q_CHECK_PTR(Child::s_vertexBuffer);

			Child::s_colorBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_colors);
			Q_CHECK_PTR(Child::s_colorBuffer);

			Child::s_indexBuffer = initBuffer(QOpenGLBuffer::IndexBuffer, Child::s_indices);
			Q_CHECK_PTR(Child::s_indexBuffer);

			auto posAttr = Child::s_program->attributeLocation("vertexPosition");
			Child::s_vertexBuffer->bind();
			Child::s_program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3);
			Child::s_program->enableAttributeArray(posAttr);

			auto colAttr = Child::s_program->attributeLocation("vertexColor");
			Child::s_colorBuffer->bind();
			Child::s_program->setAttributeBuffer(colAttr, GL_FLOAT, 0, 3);
			Child::s_program->enableAttributeArray(colAttr);
		}
	}

	prop(QVariantList, colors);

protected:
	// Instances
	static int s_instances;
	static QOpenGLShaderProgram* s_program;
	static QOpenGLVertexArrayObject* s_vao;
	static QOpenGLBuffer* s_vertexBuffer;
	static QOpenGLBuffer* s_colorBuffer;
	static QOpenGLBuffer* s_indexBuffer;
	static std::vector<GLfloat> s_vertices;
	static std::vector<GLfloat> s_colors;
	static std::vector<quint32> s_indices;
};

#endif  // GEOMETRY_H
