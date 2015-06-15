// Copyright 2015 PsychoLama

#ifndef PYRAMIDE_H
#define PYRAMIDE_H

#include <geometry.hpp>

/*! \brief Un pyramide
 *
 * Une classe gérant la géometrie simple d'une pyramide.
 */
class Pyramide : public Geometry<Pyramide> {
    Q_OBJECT

public:
    explicit Pyramide(QObject* parent = nullptr);
    typedef Geometry<Pyramide> tBase;
};

Q_DECLARE_METATYPE(Pyramide)

#endif  // PYRAMIDE_H
