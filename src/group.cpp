// Copyright 2015 PsychoLama

#include <group.hpp>

Group::Group(QObject* parent) : Actor(parent) {
	// compteur d'instances pour determiner le nom du groupe
	static int instances = 1;
	setObjectName(QString(tr("Group %1")).arg(instances++));
}

void Group::draw(const DrawInfo& info) {
	// Copie les infors de rendus
	auto localInfo = info;

	// Transforme la matrice model par la transformation du groupe
	localInfo.uniforms.insert("model", qvariant_cast<QMatrix4x4>(info.uniforms.value("model")) * transform());

	// Affiche tous les enfants avec cette nouvelle matrice
	for (auto i : children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) child->draw(localInfo);
	}
}
