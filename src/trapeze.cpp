// Copyright 2015 PsychoLama

#include "trapeze.hpp"
#include <QColor>

template<>
int Trapeze::tBase::s_instances = 0;

Trapeze::Trapeze(QObject *parent) : Geometry(parent) {
	setObjectName(QString(tr("Trapeze %1")).arg(Trapeze::tBase::s_instances));
	colors({
		QColor(255, 0, 0),
		QColor(0, 255, 0)
	});
}

template<>
QOpenGLVertexArrayObject* Trapeze::tBase::s_vao = nullptr;
template<>
ProgramList Trapeze::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Trapeze::tBase::s_buffers = {};

template<>
QVector<quint32> Trapeze::tBase::s_indexBuffer = {
	0, 1, 2,		// Face 1
	2, 3, 0,		//
	6, 5, 4,		// Face 2
	4, 7, 6,		//
};

template<>
QHash<QString, QVector<GLfloat>> Trapeze::tBase::s_buffersData = {
        {"Position", {
			-1.0f,  -1.0f, 0.0f,		// Face 1
			 1.0f,  -1.0f, 0.0f,		//
			 0.75f,  1.0f, 0.0f,		//
			-0.75f,  1.0f, 0.0f,		//
			-1.0f,  -1.0f, 0.0f,		// Face 2
			 1.0f,  -1.0f, 0.0f,		//
			 0.75f,  1.0f, 0.0f,		//
			-0.75f,  1.0f, 0.0f,		//
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
        }},
        {"UV", {
			0.0f,  0.0f,		// Face 1
			0.0f,  1.0f,		//
			1.0f,  0.0f,		//
			1.0f,  1.0f,		//
			0.0f,  0.0f,		// Face 2
			0.0f,  1.0f,		//
			1.0f,  0.0f,		//
			1.0f,  1.0f,		//
        }}
};
