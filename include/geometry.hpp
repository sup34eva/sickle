// Copyright 2015 PsychoLama

#ifndef GEOMETRY_H
#define GEOMETRY_H

#include <QObject>
#include <QVector3D>
#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <globals.hpp>
#include <memory>
#include <vector>

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
class Geometry : public QObject {
	Q_OBJECT

public:
	explicit Geometry(QObject* parent = nullptr);
	void draw(const DrawInfo& info);

	propSig(QVector3D, position, moved);
	propSig(QQuaternion, orientation, rotated);
	propSig(QVector3D, scale, scaled);

protected:
	QMatrix4x4 transform();

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
	template <typename Child>
	noinline static void initProgram(QObject* parent) {
		if (Child::s_instances++ == 0) {
			Child::s_program = new QOpenGLShaderProgram(parent);

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

			Child::s_vertexBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_vertices);
			Q_CHECK_PTR(Child::s_vertexBuffer);

			Child::s_colorBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_colors);
			Q_CHECK_PTR(Child::s_colorBuffer);

			Child::s_indexBuffer = initBuffer(QOpenGLBuffer::IndexBuffer, Child::s_indices);
			Q_CHECK_PTR(Child::s_indexBuffer);

			if (!Child::s_program->bind()) {
				qCritical() << "Could not bind shader to context";
				return;
			}

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

private:
	// Instances
	static int s_instances;
	static QOpenGLShaderProgram* s_program;
	static QOpenGLBuffer* s_vertexBuffer;
	static QOpenGLBuffer* s_colorBuffer;
	static QOpenGLBuffer* s_indexBuffer;
};

#endif  // GEOMETRY_H
