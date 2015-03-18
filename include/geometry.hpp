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
#include <tuple>

/*! \brief Définit les propriétés d'un materiau
 *
 * Cette structure contient tous les paramètres passés au shader pour définir un materiau, basé sur le principe des
 * shaders Disney.
 */
struct Material : public QObject {
	Q_OBJECT
public:
	Material() {
		specular(50);
		roughness(50);
		sheenTint(50);
		clearcoatGloss(100);
	}
	prop(float, metallic);  //! Si la surface est refléchissante (ex: metal)
	prop(float, subsurface);  //! Si la surface diffuse la lumière (ex: peau, bougie)
	prop(float, specular);  //! Si la surface est brillante (ex: plastique)
	prop(float, roughness);  //! Si la surface est rugeuse (ex: bois, tissu)
	prop(float, specularTint);  //! Si la composante speculaire doit prendre la couleur de la surface
	prop(float, anisotropic);  //! Si la reflection est anisotropique (ex: soie, cheveux)
	prop(float, sheen);  //! Si la surface a un eclat supplémentaire (ex: tissu)
	prop(float, sheenTint);  //! Si l'eclat doit prendre la teinte de la surface
	prop(float, clearcoat);  //! Si la surface est vernie
	prop(float, clearcoatGloss);  //! Si le vernis est brillant
};

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
		material(new Material());
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

		auto Model = transform();
		auto View = qvariant_cast<QMatrix4x4>(info.uniforms.value("view"));
		auto Projection = qvariant_cast<QMatrix4x4>(info.uniforms.value("projection"));
		program->setUniformValue("model", Model);
		program->setUniformValue("MVP", Projection * View * Model);

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

		func->glDrawElements(info.mode, Child::s_indices.size(), GL_UNSIGNED_INT, nullptr);

		Child::s_vao->release();
		program->release();
	}

protected:
	static QVector3D triangle(int id) {
		return QVector3D(
					Child::s_indices[id],
					Child::s_indices[id + 1],
					Child::s_indices[id + 2]);
	}

	static QVector3D vertex(int id) {
		return QVector3D(
					Child::s_vertices[id],
					Child::s_vertices[id + 1],
					Child::s_vertices[id + 2]);
	}

	static QVector2D UV(int id) {
		return QVector2D(
					Child::s_uv[id],
					Child::s_uv[id + 1]);
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

			Child::s_vertexBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_vertices);
			Q_CHECK_PTR(Child::s_vertexBuffer);

			Child::s_colorBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_colors);
			Q_CHECK_PTR(Child::s_colorBuffer);

			Child::s_normalBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_normals);
			Q_CHECK_PTR(Child::s_normalBuffer);

			Child::s_UVBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_uv);
			Q_CHECK_PTR(Child::s_UVBuffer);

			Child::s_tangentBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_tangents);
			Q_CHECK_PTR(Child::s_tangentBuffer);

			Child::s_bitangentBuffer = initBuffer(QOpenGLBuffer::VertexBuffer, Child::s_bitangents);
			Q_CHECK_PTR(Child::s_bitangentBuffer);

			Child::s_indexBuffer = initBuffer(QOpenGLBuffer::IndexBuffer, Child::s_indices);
			Q_CHECK_PTR(Child::s_indexBuffer);

			for(auto shader : GeoBase::s_shaderList) {
				auto name = std::get<0>(shader);
				auto program = new QOpenGLShaderProgram(parent);

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

				program->setUniformValue("lightPower", 1.2f);
				program->setUniformValue("lightColor", QVector3D(1, 1, 1));
				program->setUniformValue("ambientColor", QVector3D(0.1, 0.1, 0.1));

				auto posAttr = program->attributeLocation("vertexPosition");
				Child::s_vertexBuffer->bind();
				program->setAttributeBuffer(posAttr, GL_FLOAT, 0, 3);
				program->enableAttributeArray(posAttr);

				auto colAttr = program->attributeLocation("vertexColor");
				Child::s_colorBuffer->bind();
				program->setAttributeBuffer(colAttr, GL_FLOAT, 0, 3);
				program->enableAttributeArray(colAttr);

				auto normAttr = program->attributeLocation("vertexNormal");
				Child::s_normalBuffer->bind();
				program->setAttributeBuffer(normAttr, GL_FLOAT, 0, 3);
				program->enableAttributeArray(normAttr);

				auto uvAttr = program->attributeLocation("vertexUV");
				Child::s_UVBuffer->bind();
				program->setAttributeBuffer(uvAttr, GL_FLOAT, 0, 2);
				program->enableAttributeArray(uvAttr);

				auto tanAttr = program->attributeLocation("vertexTangent");
				Child::s_tangentBuffer->bind();
				program->setAttributeBuffer(tanAttr, GL_FLOAT, 0, 3);
				program->enableAttributeArray(tanAttr);

				auto btanAttr = program->attributeLocation("vertexBitangent");
				Child::s_bitangentBuffer->bind();
				program->setAttributeBuffer(btanAttr, GL_FLOAT, 0, 3);
				program->enableAttributeArray(btanAttr);

				Child::s_programList.insert(name, program);
			}
		}
	}

	// Instances
	static int s_instances;
	static ProgramList s_programList;
	static QOpenGLVertexArrayObject* s_vao;
	static QOpenGLBuffer* s_vertexBuffer;
	static QOpenGLBuffer* s_colorBuffer;
	static QOpenGLBuffer* s_normalBuffer;
	static QOpenGLBuffer* s_UVBuffer;
	static QOpenGLBuffer* s_tangentBuffer;
	static QOpenGLBuffer* s_bitangentBuffer;
	static QOpenGLBuffer* s_indexBuffer;
	static QVector<GLfloat> s_vertices;
	static QVector<GLfloat> s_colors;
	static QVector<GLfloat> s_uv;
	static QVector<GLfloat> s_normals;
	static QVector<GLfloat> s_tangents;
	static QVector<GLfloat> s_bitangents;
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

	static void calcNormals() {
		Child::s_normals = Child::s_vertices;
		Child::s_tangents = Child::s_vertices;
		Child::s_bitangents = Child::s_vertices;
		auto size = Child::s_indices.size();
		for(int i = 0; i < size; i += 3) {
			auto tri = triangle(i);
			auto v = calcTriNormal(tri);
			auto tan = calcTriTangent(tri);
			auto btan = calcTriBitangent(tri);
			auto t = tri * 3;
			for(int j = 0; j < 3; j++) {
				Child::s_normals[t.x() + j] = Child::s_normals[t.y() + j] = Child::s_normals[t.z() + j] = v[j];
				Child::s_tangents[t.x() + j] = Child::s_tangents[t.y() + j] = Child::s_tangents[t.z() + j] = tan[j];
				Child::s_bitangents[t.x() + j] = Child::s_bitangents[t.y() + j] = Child::s_bitangents[t.z() + j] = btan[j];
			}
		}
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
