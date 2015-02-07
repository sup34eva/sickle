// Copyright 2015 PsychoLama

#include <group.hpp>

Group::Group(QObject* parent) : Actor(parent) {
}

void Group::draw(const DrawInfo& info) {
	auto localView = info.View * transform();
	DrawInfo localInfo{localView, info.Projection, info.mode};
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(localInfo);
	}
}
