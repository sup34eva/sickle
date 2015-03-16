// Copyright 2015 PsychoLama

#include <group.hpp>

Group::Group(QObject* parent) : Actor(parent) {
}

void Group::draw(const DrawInfo& info) {
	auto localInfo = info;
	localInfo.uniforms.insert("view", qvariant_cast<QMatrix4x4>(info.uniforms.value("view")) * transform());
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(localInfo);
	}
}
