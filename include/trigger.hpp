// Copyright 2015 PsychoLama

#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include <geometry.hpp>

/*! \brief Un volume déclencheur
 *
 * Une classe générique permettant de créér un volume n'apparaissant pas en jeu.
 * Ce volume peut être utilisé pour déclencher des evenements lorsque le joueur entre a l'interieur
 * (par exemple un changment de zone).
 */
class Trigger : public Geometry<Trigger> {
	Q_OBJECT

	public:
		explicit Trigger(QObject* parent = nullptr);
		virtual void draw(const DrawInfo &info);
		typedef Geometry<Trigger> tBase;
		prop(int, zone);
};

Q_DECLARE_METATYPE(Trigger)

#endif  // TRIGGER_HPP
