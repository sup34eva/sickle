// Copyright 2015 PsychoLama

#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include <globals.hpp>
#include <QMetaProperty>
#include <QPropertyAnimation>
#include <QtMath>

#ifdef UNIT_TEST
#define ANIM_DUR 0
#else
#define ANIM_DUR 100
#endif

/*! \brief Gestion de la camera
 *
 * Permet de calculer la matrice view d'une camera a partir de 2 angles (vertical et horizontal) et une position.
 * Les propriétés bias permettent de décaler les angles pour eviter que la caméra ne retourne a son point de départ
 * lorsque l'utilisateur la tourne.
 * La propriété speed correspond a la vitesse de déplacement de la caméra.
 */
class Camera : public QObject {
	Q_OBJECT

public:
	explicit Camera(QObject* parent = nullptr);
	explicit Camera(Camera* copy);
	void resetBias();
	QVector3D move(QVector3D val);
	QMatrix4x4 view();

	QVector3D right() {
		return QVector3D(sin(m_hAngle - 3.14f / 2.0f), 0, cos(m_hAngle - 3.14f / 2.0f));
	}

	QVector3D direction() {
		return QVector3D(cos(m_vAngle) * sin(m_hAngle), sin(m_vAngle), cos(m_vAngle) * cos(m_hAngle));
	}

	QVector3D up() {
		return QVector3D::crossProduct(right(), direction());
	}

	QQuaternion orientation() {
		QVector3D vUp = up(),
				vRight = right(),
				vDir = direction();

		QMatrix4x4 basis(
			vRight.x(), vRight.y(), vRight.z(), 0.0f,
			vUp.x(),    vUp.y(),    vUp.z(),    0.0f,
			vDir.x(),   vDir.y(),   vDir.z(),   0.0f,
			0.0f,       0.0f,       0.0f,       1.0f
		);

		qreal W = qSqrt(1.0f + basis(1, 1) + basis(2, 2) + basis(3, 3)) / 2.0f,
			scale = W * 4.0;
		return QQuaternion(
			W,
			(basis(3, 2) - basis(2, 3)) / scale,
			(basis(1, 3) - basis(3, 1)) / scale,
			(basis(2, 1) - basis(1, 2)) / scale
		);
	}

	void velocity(QVector3D v, int dur = ANIM_DUR);
	propSig(QVector3D, position, moved);
	prop(float, vAngle);
	prop(float, vBias);
	prop(float, hAngle);
	prop(float, hBias);
	prop(float, speed);
};

#endif  // CAMERA_H
