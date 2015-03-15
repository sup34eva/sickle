// Copyright 2015 PsychoLama

#include <group.hpp>

Group::Group(QObject* parent) : Actor(parent) {
}

void Group::draw(const DrawInfo& info) {
	auto localInfo = info;
	localInfo.View = info.View * transform();
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(localInfo);
	}
}
