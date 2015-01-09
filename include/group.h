#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include "geometry.h"

class Group : public Geometry
{
public:
    Group();
    ~Group();
    void draw(QMatrix4x4& View, QMatrix4x4& Projection) Q_DECL_OVERRIDE;
    int addChild(Geometry* child);
private:
    QVector<Geometry*> children;
};

#endif // GROUP_H
