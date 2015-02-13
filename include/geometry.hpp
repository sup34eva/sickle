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

		auto Model = transform();
		auto MVP = info.Projection * info.View * Model;
		Child::s_program->setUniformValue("model", Model);
		Child::s_program->setUniformValue("view", info.View);
		Child::s_program->setUniformValue("MVP", MVP);

		Child::s_program->setUniformValue("lightD", QVector3D(1, 1, 1));

		auto func = info.context->functions();
		func->glDrawElements(info.mode, Child::s_indices.size(), GL_UNSIGNED_INT, nullptr);

		Child::s_program->release();
	}

protected:
	static QVector3D triangle(int id) {
		return QVector3D(
					Child::s_indices[id],
					Child::s_indices[id + 1],
					Child::s_indices[id + 2]) * 3;
	}

	static QVector3D vertex(int id) {
		return QVector3D(
					Child::s_vertices[id],
					Child::s_vertices[id + 1],
					Child::s_vertices[id + 2]);
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

			if (!Child::s_program->addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/lit.vert")) {
				qCritical() << "Could not load vertex shader:" << Child::s_program->log();
				return;
			}

			if (!Child::s_program->addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/lit.frag")) {
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

			Child::s_program->setUniformValue("lightPower", 1.2f);
			Child::s_program->setUniformValue("lightColor", QVector3D(1, 1, 1));
			Child::s_program->setUniformValue("ambientColor", QVector3D(0.1, 0.1, 0.1));

			calcNormals();

			Child::s_vertexBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_vertices);
			Q_CHECK_PTR(Child::s_vertexBuffer);

			Child::s_colorBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_colors);
			Q_CHECK_PTR(Child::s_colorBuffer);

			Child::s_normalBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_normals);
			Q_CHECK_PTR(Child::s_normalBuffer);

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

			auto normAttr = Child::s_program->attributeLocation("vertexNormal");
			Child::s_normalBuffer->bind();
			Child::s_program->setAttributeBuffer(normAttr, GL_FLOAT, 0, 3);
			Child::s_program->enableAttributeArray(normAttr);
		}
	}

	prop(QVariantList, colors);

	// Instances
	static int s_instances;
	static QOpenGLShaderProgram* s_program;
	static QOpenGLVertexArrayObject* s_vao;
	static QOpenGLBuffer* s_vertexBuffer;
	static QOpenGLBuffer* s_colorBuffer;
	static QOpenGLBuffer* s_normalBuffer;
	static QOpenGLBuffer* s_indexBuffer;
	static QVector<GLfloat> s_vertices;
	static QVector<GLfloat> s_colors;
	static QVector<GLfloat> s_normals;
	static QVector<quint32> s_indices;

private:
	template <typename T>
	noinline static QOpenGLBuffer* initBuffer(QOpenGLBuffer::Type type, const QVector<T>& data) {
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

	static QVector3D calcTriNormal(QVector3D t) {
		auto V = vertex(t.y()) - vertex(t.x());
		auto W = vertex(t.z()) - vertex(t.x());
		QVector3D normal(
			(V.y() * W.z()) - (V.z() * W.y()),
			(V.z() * W.x()) - (V.x() * W.z()),
			(V.x() * W.y()) - (V.y() * W.x()));
		normal.normalize();
		return normal;
	}

	static void calcNormals() {
		Child::s_normals = Child::s_vertices;
		auto size = Child::s_indices.size();
		for(int i = 0; i < size; i += 3) {
			auto t = triangle(i);
			auto v = calcTriNormal(t);
			for(int j = 0; j < 3; j++)
				Child::s_normals[t.x() + j] = Child::s_normals[t.y() + j] = Child::s_normals[t.z() + j] = v[j];
		}
	}
};

#endif  // GEOMETRY_H
