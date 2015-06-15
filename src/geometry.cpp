// Copyright 2015 PsychoLama

#include <geometry.hpp>
#include <viewport.hpp>

GeoBase::GeoBase(QObject* parent) : Actor(parent), m_castShadows(true) {
	// Initialise le materiau
	material(new Material(this));

	// Et initialise le parent de la Geometry
	if(parent != nullptr)
		setParent(parent);
}

void GeoBase::setParent(QObject *parent) {
	Actor::setParent(parent);

	// Si le viewport a été trouvé
	if(viewport() != nullptr) {
		// Enregistre les evenements spécifiques a Geometry dans celui-ci
		connect(this, &GeoBase::shadowChanged, viewport(), &Viewport::updateLights);
	} else {
		qDebug() << "No Viewport found for Geometry";
	}
}

// Liste des shaders
ShaderList GeoBase::s_shaderList = {
	ShaderInfo{RB_DEPTH, ":/shaders/scene.vert", QString()},
	ShaderInfo{RB_SCENE, ":/shaders/scene.vert", ":/shaders/buffer.frag"}
};
