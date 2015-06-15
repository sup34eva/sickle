// Copyright 2015 PsychoLama

#ifndef LINE_HPP
#define LINE_HPP

#include <geometry.hpp>

/*! \brief Une ligne
 *
 * Affiche une simple ligne en 2D, reliant 2 points de controle
 */
class Line : public Geometry<Line> {
	Q_OBJECT

	public:
		explicit Line(QObject* parent = nullptr);
		virtual void draw(const DrawInfo &info);
		typedef Geometry<Line> tBase;

		prop(QVector3D, point1);
		prop(QVector3D, point2);

	protected:
		virtual QMatrix4x4 transform();
};

Q_DECLARE_METATYPE(Line)

#endif  // LINE_HPP
