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
#include <trapeze.hpp>
#include <light.hpp>
#include <spotlight.hpp>
#include <group.hpp>
#include <trigger.hpp>
#include <cercle.hpp>

DefaultActorBuilder::DefaultActorBuilder(QObject *parent) : QObject(parent) {
	// Déclare les classes d'acteurs dans le système de méta-objet de Qt (utilisé pour les sauvegardes)

	qRegisterMetaType<Cube>("Cube");
	qRegisterMetaType<Sphere>("Sphere");
	qRegisterMetaType<Pyramide>("Pyramide");
	qRegisterMetaType<Cylinder>("Cylinder");

	qRegisterMetaType<Line>("Line");
	qRegisterMetaType<Rect>("Rect");
	qRegisterMetaType<Trapeze>("Trapeze");
    qRegisterMetaType<Cercle>("Cercle");

	qRegisterMetaType<Light>("Light");
	qRegisterMetaType<Spotlight>("Spotlight");

	qRegisterMetaType<Group>("Group");
	qRegisterMetaType<Material>("Material");
	qRegisterMetaType<Trigger>("Trigger");
}

QList<ActorBuilder::ActorClass> DefaultActorBuilder::getClasses() {
	return {
		ActorClass{tr("Cube"), GEOMETRY_3D},
		ActorClass{tr("Sphere"), GEOMETRY_3D},
		ActorClass{tr("Pyramid"), GEOMETRY_3D},
		ActorClass{tr("Cylinder"), GEOMETRY_3D},

		ActorClass{tr("Line"), GEOMETRY_2D},
		ActorClass{tr("Rectangle"), GEOMETRY_2D},
        ActorClass{tr("Trapeze"), GEOMETRY_2D},
        ActorClass{tr("Cercle"), GEOMETRY_2D},

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
            return viewport->addChild<Trapeze>();
        case 7:
            return viewport->addChild<Cercle>();

        case 8:
			return viewport->addChild<Light>();
        case 9:
			return viewport->addChild<Spotlight>();

        case 10:
			return viewport->addChild<Trigger>();

		default:
			return nullptr;
	}
}
