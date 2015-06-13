// Copyright 2015 PsychoLama

#ifndef GROUP_H
#define GROUP_H

#include <QVector>
#include <geometry.hpp>

/*! \brief Groupe de geometries
 *
 * Toute les géometries enfant de cet objet seront affichés avec une transformation relative a celle du groupe.
 */
class Group : public Actor {
	Q_OBJECT

public:
	explicit Group(QObject* parent = nullptr);
	virtual void draw(const DrawInfo& info);
};

Q_DECLARE_METATYPE(Group)

#endif  // GROUP_H
