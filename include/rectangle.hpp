// Copyright 2015 PsychoLama

#ifndef RECT_H
#define RECT_H

#include <geometry.hpp>


class Rect : public Geometry<Rect> {
    Q_OBJECT

	public:
		explicit Rect(QObject* parent = nullptr);
		typedef Geometry<Rect> tBase;
};

Q_DECLARE_METATYPE(Rect)

#endif  // RECT_H
