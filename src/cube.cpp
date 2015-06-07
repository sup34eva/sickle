// Copyright 2015 PsychoLama

#include <cube.hpp>
#include <vector>

template<>
int Cube::tBase::s_instances = 0;

Cube::Cube(QObject* parent) : Geometry(parent) {
	setObjectName(QString(tr("Cube %1")).arg(Cube::tBase::s_instances));
	colors({
		QColor(255,	0,		0),
		QColor(255,	255, 	0),
		QColor(0,	255, 	0),
		QColor(0,	255, 	255),
		QColor(0,	0,		255),
		QColor(255,	0,		255)
	});
}

template<>
QOpenGLVertexArrayObject* Cube::tBase::s_vao = nullptr;
template<>
ProgramList Cube::tBase::s_programList = {};
template<>
QHash<QString, QOpenGLBuffer*> Cube::tBase::s_buffers = {};

template<>
QVector<quint32> Cube::tBase::s_indexBuffer = {
	0,  1,  3,		// Face 1
	3,  1,  2,		//
	4,  5,  7,		// Face 2
	7,  5,  6,		//
	8,  9,  11,		// Face 3
	11, 9,  10,		//
	12, 13, 15,		// Face 4
	15, 13, 14,		//
	16, 17, 19,		// Face 5
	19, 17, 18,		//
	20, 21, 23,		// Face 6
	23, 21, 22,		//
};

template<>
QHash<QString, QVector<GLfloat>> Cube::tBase::s_buffersData = {
	{"Position", {
		-1.0f, -1.0f, -1.0f,		// Face 1
		-1.0f, -1.0f,  1.0f,		//
		-1.0f,  1.0f,  1.0f,		//
		-1.0f,  1.0f, -1.0f,		//
		 1.0f, -1.0f, -1.0f,		// Face 2
		 1.0f,  1.0f, -1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f, -1.0f,		// Face 3
		 1.0f, -1.0f, -1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f,  1.0f,		//
		-1.0f,  1.0f, -1.0f,		// Face 4
		-1.0f,  1.0f,  1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		 1.0f,  1.0f, -1.0f,		//
		-1.0f, -1.0f, -1.0f,		// Face 5
		-1.0f,  1.0f, -1.0f,		//
		 1.0f,  1.0f, -1.0f,		//
		 1.0f, -1.0f, -1.0f,		//
		-1.0f, -1.0f,  1.0f,		// Face 6
		 1.0f, -1.0f,  1.0f,		//
		 1.0f,  1.0f,  1.0f,		//
		-1.0f,  1.0f,  1.0f,		//
	}},
	{"Color", {
		0.0f,		// Face 1
		0.0f,		//
		0.0f,		//
		0.0f,		//
		1.0f,		// Face 2
		1.0f,		//
		1.0f,		//
		1.0f,		//
		2.0f,		// Face 3
		2.0f,		//
		2.0f,		//
		2.0f,		//
		3.0f,		// Face 4
		3.0f,		//
		3.0f,		//
		3.0f,		//
		4.0f,		// Face 5
		4.0f,		//
		4.0f,		//
		4.0f,		//
		5.0f,		// Face 6
		5.0f,		//
		5.0f,		//
		5.0f,		//
	}},
	{"UV", {
		0.0f,  0.0f,		// Face 1
		0.0f,  1.0f,		//
		1.0f,  1.0f,		//
		1.0f,  0.0f,		//
		0.0f,  0.0f,		// Face 2
		1.0f,  0.0f,		//
		1.0f,  1.0f,		//
		0.0f,  1.0f,		//
		0.0f,  0.0f,		// Face 3
		1.0f,  0.0f,		//
		1.0f,  1.0f,		//
		0.0f,  1.0f,		//
		0.0f,  0.0f,		// Face 4
		0.0f,  1.0f,		//
		1.0f,  1.0f,		//
		1.0f,  0.0f,		//
		0.0f,  0.0f,		// Face 5
		0.0f,  1.0f,		//
		1.0f,  1.0f,		//
		1.0f,  0.0f,		//
		0.0f,  0.0f,		// Face 6
		1.0f,  0.0f,		//
		1.0f,  1.0f,		//
		0.0f,  1.0f,		//
	}}
};
