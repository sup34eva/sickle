// Copyright 2015 PsychoLama

#ifndef PYRAMIDE_H
#define PYRAMIDE_H

#include <geometry.hpp>

class Pyramide : public Geometry<Pyramide> {
    Q_OBJECT

public:
    explicit Pyramide(QObject* parent = nullptr);
    typedef Geometry<Pyramide> tBase;
};

Q_DECLARE_METATYPE(Pyramide)

#endif  // PYRAMIDE_H
