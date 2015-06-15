// Copyright 2015 PsychoLama

#ifndef ACTORBUILDER_HPP
#define ACTORBUILDER_HPP

#include <QObject>
#include <QString>
#include <QMetaObject>
#include <QList>
#include <tuple>

class Viewport;
class Actor;

/*! \brief Interface de création d'Acteurs
 *
 * Les classes implémentant cette interface permettent d'ajouter des acteurs a une zone.
 */
class ActorBuilder {
	public:
		virtual ~ActorBuilder() {}

		typedef enum ActorType {
			GEOMETRY_3D,
			GEOMETRY_2D,
			LIGHT,
			MISC
		} ActorType;

		typedef std::tuple<QString, ActorType> ActorClass;

		virtual QList<ActorClass> getClasses() = 0;
		virtual Actor* build(Viewport*, int) = 0;
};

Q_DECLARE_INTERFACE(ActorBuilder, "com.sup3asc2.sickle.ActorBuilder/1.0")

/*! \brief Generateur d'acteur par defaut
 *
 * Permet de créér les acteurs de base de Sickle
 */
class DefaultActorBuilder : public QObject, public ActorBuilder {
	Q_OBJECT
	Q_INTERFACES(ActorBuilder)

	public:
		explicit DefaultActorBuilder(QObject* parent = nullptr);
		virtual QList<ActorClass> getClasses();
		virtual Actor* build(Viewport* viewport, int index);
};

#endif  // ACTORBUILDER_HPP
