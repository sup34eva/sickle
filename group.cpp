#include "group.h"

Group::Group()
{

}

Group::~Group()
{

}

void Group::draw(QMatrix4x4& View, QMatrix4x4& Projection) {
    View *= transform();
    for(auto child : children) {
        child->draw(View, Projection);
    }
}

int Group::addChild(Geometry* child) {
    auto index = children.indexOf(child);
    if(index == -1) {
        child->orientation(child->orientation() - orientation());
        child->scale(child->scale() - scale());
        child->position(child->position() - position());
        children.append(child);
        return children.length() - 1;
    } else {
        return index;
    }
}
