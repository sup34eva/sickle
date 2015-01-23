#include <group.hpp>

Group::Group(QObject* parent) : Geometry(parent)
{

}

void Group::draw(const DrawInfo& info) {
    auto localView = info.View * transform();
    DrawInfo localInfo{localView, info.Projection, info.mode};
    for(auto i : children()) {
        auto child = dynamic_cast<Geometry*>(i);
        if(child)
            child->draw(localInfo);
    }
}
