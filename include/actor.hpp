// Copyright 2015 PsychoLama

#ifndef ACTOR_HPP
#define ACTOR_HPP

#include <globals.hpp>
#include <QMatrix4x4>
#include <QOpenGLFunctions>

typedef struct DrawInfo {
	QMatrix4x4 View;
	QMatrix4x4 Projection;
	GLenum mode;
	QOpenGLContext* context;
} DrawInfo;

class Actor : public QObject {
	Q_OBJECT

	public:
		explicit Actor(QObject* parent = nullptr);
		virtual void draw(const DrawInfo& info) = 0;

		propSig(QVector3D, position, moved);
		propSig(QQuaternion, orientation, rotated);
		propSig(QVector3D, scale, scaled);
	protected:
		virtual QMatrix4x4 transform();
};

#endif  // ACTOR_HPP

