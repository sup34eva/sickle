// Copyright 2015 PsychoLama

#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <QColor>
#include <actor.hpp>

class Viewport;

/*! \brief Une lumière directionnelle
 *
 * Cette lumière est représentée uniquement par sa direction, elle est considérée comme étant infiniment loins.
 * Elle convient donc pour représenter le soleil, ou toute source de lumière globale.
 *
 * Il est possible de modifier la taille de la shadowmap et les valeurs de profondeur pour n'éclairer que les zones
 * faisant partie du monde (plus la shadowmap est petité et meilleure sera la qualité des ombres).
 */
class Light : public Actor {
	Q_OBJECT

public:
	explicit Light(QObject* parent = nullptr);
	~Light();
	virtual void setParent(QObject* parent);
	virtual void draw(const DrawInfo& info) {
		Q_UNUSED(info)
	}

	prop(float, power);
	prop(QColor, color);
	propSig(float, nearZ, nearZChanged);
	propSig(float, farZ, farZChanged);
	propSig(QSize, mapSize, mapSizeChanged);

	QVector3D direction();
	QMatrix4x4 depth();

	GLuint& getBuffer() {
		return m_buffer;
	}

	GLuint& getTexture() {
		return m_texture;
	}

	QMatrix4x4 projection() {
		return m_projection;
	}

	QMatrix4x4 view() {
		return m_view;
	}

	virtual int type() {
		return 0;
	}

	virtual void updateProj();
	virtual void updateView();

public slots:
	void update();

protected:
	QMatrix4x4 m_projection;
	QMatrix4x4 m_view;
	GLuint m_buffer;
	GLuint m_texture;
};

Q_DECLARE_METATYPE(Light)

#endif  // LIGHT_HPP
