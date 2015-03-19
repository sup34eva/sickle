#ifndef SPOTLIGHT_HPP
#define SPOTLIGHT_HPP

#include <light.hpp>

class Spotlight : public Light {
	Q_OBJECT

public:
	Spotlight(QObject* parent = nullptr);

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

#endif // SPOTLIGHT_HPP
