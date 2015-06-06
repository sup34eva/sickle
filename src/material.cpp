// Copyright 2015 PsychoLama

#include <material.hpp>

Material::Material(QObject *parent) : QObject(parent) {
	specular(50);
	roughness(50);
	sheenTint(50);
	clearcoatGloss(100);
}

