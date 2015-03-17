#ifndef LIGHT_HPP
#define LIGHT_HPP

#include <QColor>
#include <actor.hpp>

class Viewport;

class Light : public Actor {
	Q_OBJECT

public:
	Light(QObject* parent = nullptr);
	virtual void setParent(QObject*);
	virtual void draw(const DrawInfo&) {}

	prop(QMatrix4x4, projection);
	prop(QMatrix4x4, view);
	prop(GLuint, buffer);
	prop(GLuint, texture);
	prop(float, power);
	prop(QColor, color);
	propSig(float, nearZ, nearZChanged);
	propSig(float, farZ, farZChanged);
	propSig(QSize, mapSize, mapSizeChanged);

	QVector3D direction();
	QMatrix4x4 depth();

	GLuint* getBuffer() {
		return &m_buffer;
	}

	GLuint* getTexture() {
		return &m_texture;
	}

public slots:
	virtual void updateProj();
	void updateView();
	void update();
};

Q_DECLARE_METATYPE(Light)

#endif // LIGHT_HPP
