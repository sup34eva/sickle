#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include "globals.hpp"
#include <QMetaProperty>

/*!
 * \class Camera
 * \brief Gestion de la camera
 *
 * Permet de calculer la matrice view d'une camera a partir de 2 angles (vertical et horizontal) et une position.
 * Les propriétés bias permettent de déclaer les angles pour eviter que la caméra ne retourne a son point de départ
 * lorsque l'utilisateur la tourne.
 * La propriété speed correspond a la vitesse de déplacement de la caméra.
 */
class Camera : public QObject {
	Q_OBJECT
public:
	Camera(QObject* parent = nullptr);
	Camera(Camera* copy);
	~Camera();
	void resetBias();
	QVector3D move(QVector3D val);
	QVector3D right();
	QVector3D direction();
	QVector3D up();
	QMatrix4x4 view();
	propSig(QVector3D, position, moved);
	prop(float, vAngle);
	prop(float, vBias);
	prop(float, hAngle);
	prop(float, hBias);
	prop(float, speed);
};

#endif  // CAMERA_H
