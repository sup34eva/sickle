// Copyright 2015 PsychoLama

#ifndef SPHERE_HPP
#define SPHERE_HPP

#include <geometry.hpp>
#include <QMetaProperty>
#include <QColor>

/*! \brief Une simple sphere
 * \todo Générer les vertices a la compilation.
 *
 * Cette classe affiche la géometrie d'un sphere. Les vertices sont généré dynamiquement pour aléger le code.
 */
class Sphere : public Geometry<Sphere> {
	Q_OBJECT

public:
	explicit Sphere(QObject* parent = nullptr);
	typedef Geometry<Sphere> tBase;
};

Q_DECLARE_METATYPE(Sphere)

#endif  // SPHERE_HPP
