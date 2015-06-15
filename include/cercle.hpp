// Copyright 2015 PsychoLama

#ifndef CERCLE_HPP
#define CERCLE_HPP

#include <geometry.hpp>

class Cercle : public Geometry<Cercle> {
    Q_OBJECT

    public:
        explicit Cercle(QObject* parent = nullptr);
        typedef Geometry<Cercle> tBase;
};

Q_DECLARE_METATYPE(Cercle)

#endif  // CERCLE_HPP
