// Copyright 2015 PsychoLama

#ifndef TRAPEZE_H
#define TRAPEZE_H

#include <geometry.hpp>

/*! \brief Un trapèze
 *
 * Une classe gérant la géometrie simple d'un trapèze.
 */
class Trapeze : public Geometry<Trapeze> {
    Q_OBJECT

public:
    explicit Trapeze(QObject* parent = nullptr);
	typedef Geometry<Trapeze> tBase;
};

Q_DECLARE_METATYPE(Trapeze)

#endif  // TRAPEZE_H
