// Copyright 2015 PsychoLama

#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <light.hpp>

/*! \brief Un projecteur
 *
 * Représente une source de lumière ponctuelle mais dirigée.
 * Utile pour modeliser un projecteur éclairant une zone donnée.
 */
class Spotlight : public Light {
	Q_OBJECT

public:
	explicit Spotlight(QObject* parent = nullptr);

	virtual int type() {
		return 1;
	}

	propSig(float, outerAngle, outerAngleChanged);
	propSig(float, innerAngle, innerAngleChanged);

public slots:
	virtual void updateProj();
	virtual void updateView();
};

Q_DECLARE_METATYPE(Spotlight)

#endif  // SPOTLIGHT_HPP
