#include <group.h>

Group::Group(QObject* parent) : Geometry(parent)
{

}

void Group::draw(QMatrix4x4& View, QMatrix4x4& Projection) {
    auto localView = View * transform();
    for(auto i : children()) {
        auto child = dynamic_cast<Geometry*>(i);
        if(child)
            child->draw(localView, Projection);
    }
}
