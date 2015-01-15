#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include <geometry.h>

class Group : public Geometry
{
public:
    Group(QObject* parent = nullptr);
    void draw(QMatrix4x4& View, QMatrix4x4& Projection) Q_DECL_OVERRIDE;
};

#endif // GROUP_H
