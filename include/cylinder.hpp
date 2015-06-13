// Copyright 2015 PsychoLama

#ifndef CYLINDER_HPP
#define CYLINDER_HPP

#include <geometry.hpp>

class Cylinder : public Geometry<Cylinder> {
	Q_OBJECT

	public:
		explicit Cylinder(QObject* parent = nullptr);
		typedef Geometry<Cylinder> tBase;
};

Q_DECLARE_METATYPE(Cylinder)

#endif   // CYLINDER_HPP
