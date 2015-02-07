// Copyright 2015 PsychoLama

#ifndef CUBE_H
#define CUBE_H

#include <geometry.hpp>
#include <globals.hpp>
#include <QColor>
#include <QMetaProperty>
#include <vector>

/*! \brief Un simple cube
 *
 * Une classe gérant la géometrie simple d'un cube.
 */
class Cube : public Geometry<Cube> {
	Q_OBJECT

public:
	explicit Cube(QObject* parent = nullptr);
	typedef Geometry<Cube> tBase;
};

Q_DECLARE_METATYPE(Cube)

#endif  // CUBE_H
