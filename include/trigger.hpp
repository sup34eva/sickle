// Copyright 2015 PsychoLama

#ifndef TRIGGER_HPP
#define TRIGGER_HPP

#include <geometry.hpp>

class Trigger : public Geometry<Trigger> {
	Q_OBJECT

	public:
		explicit Trigger(QObject* parent = nullptr);
		virtual void draw(const DrawInfo &info);
		typedef Geometry<Trigger> tBase;
		prop(int, zone);
};

Q_DECLARE_METATYPE(Trigger)

#endif  // TRIGGER_HPP
