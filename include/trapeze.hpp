// Copyright 2015 PsychoLama

#ifndef TRAPEZE_H
#define TRAPEZE_H

#include <geometry.hpp>

class Trapeze : public Geometry<Trapeze> {
    Q_OBJECT

public:
    explicit Trapeze(QObject* parent = nullptr);
    typedef Geometry<Trapeze> tBase;
    virtual void draw(const DrawInfo &info);
};

Q_DECLARE_METATYPE(Trapeze)

#endif // TRAPEZE_H
