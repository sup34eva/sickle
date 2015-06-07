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
#include <material.hpp>
#include <memory>
#include <vector>
#include <tuple>

typedef std::tuple<RenderBuffer, QString, QString> ShaderInfo;
typedef QVector<ShaderInfo> ShaderList;
typedef QHash<RenderBuffer, QOpenGLShaderProgram*> ProgramList;

/*! \brief Base pour la classe Geometry
 *
 * Cette classe sert de base a la classe Geometry. Elle est nécéssaire car la système de meta-objet de Qt ne supporte
 * pas les classes template, toutes les propriétés de la classe Geometry sont donc déclarées ici.
 */
class GeoBase : public Actor {
	Q_OBJECT

public:
	explicit GeoBase(QObject* parent = nullptr) : Actor(parent) {
		material(new Material(this));
	}

	/*! \var m_colors
	 * \brief Liste des couleurs
	 *
	 * Liste des couleurs utilisées pour les différentes faces de la géometrie
	 */
	prop(QVariantList, colors);
	/*! \var m_material
	 * \brief Materiau de la geometrie
	 *
	 * Propriétés de la surface de la geometrie
	 */
	prop(QObject*, material);
	static ShaderList s_shaderList;
};

/*! \brief Base de toutes les géometries
 * \tparam Child La classe heritant de Geometry (utilisé pour initialiser les membres statiques)
 *
 * Cette classe est une base gerant de rendu d'un buffer de vertices.
 * Elle dispose de 3 buffers (s_vertices, s_colors et s_indices) devant être remplis par les classes enfant pour
 * afficher un objet.
 *
 */
template <typename Child>
class Geometry : public GeoBase {
public:
	explicit Geometry(QObject* parent = nullptr) : GeoBase(parent) {
		initProgram(parent);
	}
	noinline void draw(const DrawInfo& info) {
		auto func = info.context->functions();
		auto program = Child::s_programList.value(info.buffer);
		program->bind();
		Child::s_vao->bind();

		for(auto i = info.uniforms.constBegin(); i != info.uniforms.constEnd(); ++i) {
			auto loc = program->uniformLocation(i.key());
			switch(static_cast<QMetaType::Type>(i.value().type())) {
				case QMetaType::QVector3D:
					program->setUniformValue(loc, qvariant_cast<QVector3D>(i.value()));
					break;
				case QMetaType::QMatrix4x4:
					program->setUniformValue(loc, qvariant_cast<QMatrix4x4>(i.value()));
					break;
				case QMetaType::Float:
					program->setUniformValue(loc, i.value().toFloat());
					break;
				default:
					qDebug() << "Unknown uniform type" << i.value().type();
					break;
			}
		}

		for(int i = 0; i < colors().size(); i++) {
			auto val = qvariant_cast<QColor>(colors().at(i));
			program->setUniformValue(i + 2, val);
		}

		auto Model = transform();
		auto View = qvariant_cast<QMatrix4x4>(info.uniforms.value("view"));
		auto Projection = qvariant_cast<QMatrix4x4>(info.uniforms.value("projection"));
		program->setUniformValue(0, Model);
		program->setUniformValue(1, Projection * View * Model);

		if(info.buffer == RB_SCENE) {
			auto Depth = qvariant_cast<QMatrix4x4>(info.uniforms.value("depth"));
			program->setUniformValue("depth", Depth * Model);
		}

		auto mat = material()->metaObject();
		for(int i = mat->propertyOffset(); i < mat->propertyCount(); i++) {
			auto name = mat->property(i).name();
			auto loc = program->uniformLocation(QString("material.%1").arg(name));
			program->setUniformValue(loc, material()->property(name).toFloat() / 100.0f);
		}

		func->glDrawElements(info.mode, Child::s_indexBuffer.size(), GL_UNSIGNED_INT, nullptr);

		Child::s_vao->release();
		program->release();
	}

protected:
	static QVector3D triangle(int id) {
		return QVector3D(
					Child::s_indexBuffer.at(id),
					Child::s_indexBuffer.at(id + 1),
					Child::s_indexBuffer.at(id + 2));
	}

	static QVector3D vertex(int id) {
		auto pos = Child::s_buffersData.value("Position");
		return QVector3D(
					pos.at(id),
					pos.at(id + 1),
					pos.at(id + 2));
	}

	static QVector2D UV(int id) {
		auto uv = Child::s_buffersData.value("UV");
		return QVector2D(
					uv.at(id),
					uv.at(id + 1));
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
			calcNormals();

			Child::s_vao = new QOpenGLVertexArrayObject(parent);
			Child::s_vao->create();
			Child::s_vao->bind();

			auto indexB = initBuffer(QOpenGLBuffer::IndexBuffer, Child::s_indexBuffer);
			Q_CHECK_PTR(indexB);
			Child::s_buffers.insert("Index", indexB);

			for(auto it = Child::s_buffersData.constBegin(); it != Child::s_buffersData.constEnd(); ++it) {
				auto buffer = initBuffer(QOpenGLBuffer::VertexBuffer, it.value());
				Q_CHECK_PTR(buffer);
				Child::s_buffers.insert(it.key(), buffer);
			}

			for(auto shader : GeoBase::s_shaderList) {
				auto name = std::get<0>(shader);
				auto program = new QOpenGLShaderProgram(parent);

				qDebug() << "Loading shader" << name << "from files:" << std::get<1>(shader) << std::get<2>(shader);

				if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex, std::get<1>(shader))) {
					qWarning() << "Could not load vertex shader:" << program->log();
					continue;
				}

				if(name != RB_DEPTH) {
					if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment, std::get<2>(shader))) {
						qWarning() << "Could not load fragment shader:" << program->log();
						continue;
					}
				}

				if (!program->link()) {
					qWarning() << "Could not link shaders:" << program->log();
					continue;
				}

				if (!program->bind()) {
					qWarning() << "Could not bind shader to context";
					continue;
				}

				for(auto it = Child::s_buffers.constBegin(); it != Child::s_buffers.constEnd(); ++it) {
					if(it.key() != "Index") {
						auto tupleSize = it.key() == "Color" ? 1 : (it.key() == "UV" ? 2 : 3);
						QString name = "vertex" + it.key();
						auto attr = program->attributeLocation(name);
						if(attr != -1) {
							it.value()->bind();
							program->setAttributeBuffer(attr, GL_FLOAT, 0, tupleSize);
							program->enableAttributeArray(attr);
						} else {
							qWarning().noquote() << "Attribute not found:" << name;
						}
					}
				}

				Child::s_programList.insert(name, program);
			}
		}
	}

	// Instances
	static int s_instances;
	static ProgramList s_programList;
	static QOpenGLVertexArrayObject* s_vao;
	static QHash<QString, QOpenGLBuffer*> s_buffers;
	static QHash<QString, QVector<GLfloat>> s_buffersData;
	static QVector<quint32> s_indexBuffer;

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

	static void calcNormals() {
		auto normals = Child::s_buffersData.value("Position");
		auto tangents = normals;
		auto bitangents = normals;
		auto size = Child::s_indexBuffer.size();
		for(int i = 0; i < size; i += 3) {
			auto tri = triangle(i);
			auto v = calcTriNormal(tri);
			auto tan = calcTriTangent(tri);
			auto btan = calcTriBitangent(tri);
			auto t = tri * 3;
			for(int j = 0; j < 3; j++) {
				normals[t.x() + j] = normals[t.y() + j] = normals[t.z() + j] = v[j];
				tangents[t.x() + j] = tangents[t.y() + j] = tangents[t.z() + j] = tan[j];
				bitangents[t.x() + j] = bitangents[t.y() + j] = bitangents[t.z() + j] = btan[j];
			}
		}

		Child::s_buffersData.insert("Normal", normals);
		Child::s_buffersData.insert("Tangent", tangents);
		Child::s_buffersData.insert("Bitangent", bitangents);
	}

	static QVector3D calcTriNormal(QVector3D tri) {
		auto t = tri * 3;
		auto V = vertex(t.y()) - vertex(t.x());
		auto W = vertex(t.z()) - vertex(t.x());
		QVector3D normal(
			(V.y() * W.z()) - (V.z() * W.y()),
			(V.z() * W.x()) - (V.x() * W.z()),
			(V.x() * W.y()) - (V.y() * W.x()));
		normal.normalize();
		return normal;
	}

	static QVector3D calcTriTangent(QVector3D tri) {
		auto verT = tri * 3;
		auto V = vertex(verT.y()) - vertex(verT.x());
		auto W = vertex(verT.z()) - vertex(verT.x());
		auto uvT = tri * 2;
		auto UV1 = UV(uvT.y()) - UV(uvT.x());
		auto UV2 = UV(uvT.z()) - UV(uvT.x());
		auto r = 1.0f / (UV1.x() * UV2.y() - UV1.y() * UV2.x());
		return (V * UV2.y() - W * UV1.y()) * r;
	}

	static QVector3D calcTriBitangent(QVector3D tri) {
		auto verT = tri * 3;
		auto V = vertex(verT.y()) - vertex(verT.x());
		auto W = vertex(verT.z()) - vertex(verT.x());
		auto uvT = tri * 2;
		auto UV1 = UV(uvT.y()) - UV(uvT.x());
		auto UV2 = UV(uvT.z()) - UV(uvT.x());
		auto r = 1.0f / (UV1.x() * UV2.y() - UV1.y() * UV2.x());
		auto btan = (W * UV1.x() - V * UV2.x()) * r;
		return btan;
	}
};

#endif  // GEOMETRY_H
