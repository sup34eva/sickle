// Copyright 2015 PsychoLama

#include <pyramide.hpp>
#include <QColor>

template<>
int Pyramide::tBase::s_instances = 0;

Pyramide::Pyramide(QObject *parent) : Geometry(parent) {
    setObjectName(QString(tr("Pyramide %1")).arg(Pyramide::tBase::s_instances));
    colors({
		QColor(255, 0,   0),
		QColor(255, 255, 0),
		QColor(0,   255, 0),
		QColor(0,   255, 255),
		QColor(0,   0,   255),
    });
}


template<>
QOpenGLVertexArrayObject* Pyramide::tBase::s_vao = nullptr;
template<>
ProgramList Pyramide::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Pyramide::tBase::s_buffers = {};

template<>
QVector<quint32> Pyramide::tBase::s_indexBuffer = {
	0,  1,  2,		// Face 1
	3,  4,  5,		// Face 2
	8,  7,  6,		// Face 3
	11, 10, 9,		// Face 4
	12, 13, 14,		// Face 5
	12, 14, 15,		//
};

template<>
QHash<QString, QVector<GLfloat>> Pyramide::tBase::s_buffersData = {
	{"Position", {
		 0.0f,  1.0f,  0.0f,		// Face 1
		-1.0f, -1.0f,  1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		 0.0f,  1.0f,  0.0f,		// Face 2
		 1.0f, -1.0f,  1.0f,		//
		 1.0f, -1.0f, -1.0f,		//
		 0.0f,  1.0f,  0.0f,		// Face 3
		-1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f, -1.0f,		//
		 0.0f,  1.0f,  0.0f,		// Face 4
		-1.0f, -1.0f, -1.0f,		//
		 1.0f, -1.0f, -1.0f,		//
		-1.0f, -1.0f, -1.0f,		// Face 5
		 1.0f, -1.0f, -1.0f,		//
		 1.0f, -1.0f,  1.0f,		//
		-1.0f, -1.0f,  1.0f,		//
	}},
	{"Color", {
		 0.0f,		// Face 1
		 0.0f,		//
		 0.0f,		//
		 1.0f,		// Face 2
		 1.0f,		//
		 1.0f,		//
		 2.0f,		// Face 3
		 2.0f,		//
		 2.0f,		//
		 3.0f,		// Face 4
		 3.0f,		//
		 3.0f,		//
		 4.0f,		// Face 5
		 4.0f,		//
		 4.0f,		//
		 4.0f,		//
	}},
	{"UV", {
		 0.0f,  0.5f,		// Face 1
		 1.0f,  0.0f,		//
		 1.0f,  1.0f,		//
		 0.0f,  0.5f,		// Face 2
		 1.0f,  0.0f,		//
		 1.0f,  1.0f,		//
		 0.0f,  0.5f,		// Face 3
		 1.0f,  0.0f,		//
		 1.0f,  1.0f,		//
		 0.0f,  0.5f,		// Face 4
		 1.0f,  0.0f,		//
		 1.0f,  1.0f,		//
		 0.0f,  0.0f,		// Face 5
		 0.0f,  1.0f,		//
		 1.0f,  1.0f,		//
		 1.0f,  0.0f,		//
	}}
};
