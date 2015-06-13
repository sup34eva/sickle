// Copyright 2015 PsychoLama

#include <material.hpp>

Material::Material(QObject *parent) : QObject(parent) {
	metallic(0);
	subsurface(0);
	specular(50);
	roughness(50);
	specularTint(0);
	anisotropic(0);
	sheen(0);
	sheenTint(50);
	clearcoat(0);
	clearcoatGloss(100);
}

