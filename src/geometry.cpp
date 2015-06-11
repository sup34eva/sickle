// Copyright 2015 PsychoLama

#include <geometry.hpp>
#include <viewport.hpp>

GeoBase::GeoBase(QObject* parent) : Actor(parent), m_castShadows(true) {
	material(new Material(this));

	if(viewport() != nullptr) {
		connect(this, &GeoBase::shadowChanged, viewport(), &Viewport::updateLights);
	} else {
		qDebug() << "wat";
	}
}

ShaderList GeoBase::s_shaderList = {
	ShaderInfo{RB_DEPTH, ":/shaders/scene.vert", QString()},
	ShaderInfo{RB_SCENE, ":/shaders/scene.vert", ":/shaders/buffer.frag"}
};
