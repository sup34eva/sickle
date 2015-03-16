// Copyright 2015 PsychoLama

#include <geometry.hpp>

ShaderList GeoBase::s_shaderList = {
	ShaderInfo{RB_DEPTH, ":/shaders/scene.vert", QString()},
	ShaderInfo{RB_SCENE, ":/shaders/scene.vert", ":/shaders/buffer.frag"}
};
