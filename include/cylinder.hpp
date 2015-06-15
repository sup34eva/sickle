// Copyright 2015 PsychoLama

#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometry.hpp>

/*! \brief Un simple cylindre
 *
 * Les vertices sont générés par un algorithme (similaire a celui de génération de cercle)
 */
class Cylinder : public Geometry<Cylinder> {
	Q_OBJECT

	public:
		explicit Cylinder(QObject* parent = nullptr);
		typedef Geometry<Cylinder> tBase;
};

Q_DECLARE_METATYPE(Cylinder)

#endif   // CYLINDER_HPP
