// Copyright 2015 PsychoLama

#include <group.hpp>

Group::Group(QObject* parent) : Actor(parent) {
	static int instances = 1;
	setObjectName(QString(tr("Group %1")).arg(instances++));
}

void Group::draw(const DrawInfo& info) {
	auto localInfo = info;
	localInfo.uniforms.insert("model", qvariant_cast<QMatrix4x4>(info.uniforms.value("model")) * transform());
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(localInfo);
	}
}
