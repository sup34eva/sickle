#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QVector3D>
#include <QMatrix4x4>
#include "globals.h"
#include <QMetaProperty>

class Camera : public QObject
{
    Q_OBJECT
public:
    Camera(QObject* parent = nullptr);
    Camera(Camera *copy);
    ~Camera();
    void resetBias();
    QVector3D move(QVector3D val);
    QVector3D right();
    QVector3D direction();
    QVector3D up();
    QMatrix4x4 view();
    propSig(QVector3D, position, moved)
    prop(float, vAngle)
    prop(float, vBias)
    prop(float, hAngle)
    prop(float, hBias)
    prop(float, speed)
};

QDataStream &operator<<(QDataStream &, const QObject &);
QDataStream &operator>>(QDataStream &, QObject &);

#endif // CAMERA_H
