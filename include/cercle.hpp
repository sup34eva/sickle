// Copyright 2015 PsychoLama

#ifndef CERCLE_HPP
#define CERCLE_HPP

#include <geometry.hpp>

/*! \brief Un cercle en 2D
 *
 * Un cercle plat (en 2D), doté de deux cotés coloriés indépendaments.
 * Les vertices sont générés par un algorithme (similaire a celui de génération de cylindres)
 */
class Cercle : public Geometry<Cercle> {
    Q_OBJECT

    public:
        explicit Cercle(QObject* parent = nullptr);
        typedef Geometry<Cercle> tBase;
};

Q_DECLARE_METATYPE(Cercle)

#endif  // CERCLE_HPP
