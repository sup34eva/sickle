// Copyright 2015 PsychoLama

#include <actorbuilder.hpp>
#include <actor.hpp>
#include <viewport.hpp>

#include <cube.hpp>
#include <sphere.hpp>
#include <pyramide.hpp>
#include <cylinder.hpp>
#include <line.hpp>
#include <rectangle.hpp>
#include <light.hpp>
#include <spotlight.hpp>
#include <trigger.hpp>

QList<ActorBuilder::ActorClass> DefaultActorBuilder::getClasses() {
	return {
		ActorClass{tr("Cube"), GEOMETRY_3D},
		ActorClass{tr("Sphere"), GEOMETRY_3D},
		ActorClass{tr("Pyramid"), GEOMETRY_3D},
		ActorClass{tr("Cylinder"), GEOMETRY_3D},

		ActorClass{tr("Line"), GEOMETRY_2D},
		ActorClass{tr("Rectangle"), GEOMETRY_2D},

		ActorClass{tr("Light"), LIGHT},
		ActorClass{tr("Spotlight"), LIGHT},

		ActorClass{tr("Trigger"), MISC}
	};
}

Actor* DefaultActorBuilder::build(Viewport* viewport, int index) {
	switch(index) {
		case 0:
			return viewport->addChild<Cube>();
		case 1:
			return viewport->addChild<Sphere>();
		case 2:
			return viewport->addChild<Pyramide>();
		case 3:
			return viewport->addChild<Cylinder>();

		case 4:
			return viewport->addChild<Line>();
		case 5:
			return viewport->addChild<Rect>();

		case 6:
			return viewport->addChild<Light>();
		case 7:
			return viewport->addChild<Spotlight>();

		case 8:
			return viewport->addChild<Trigger>();

		default:
			return nullptr;
	}
}
