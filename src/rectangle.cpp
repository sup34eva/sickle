// Copyright 2015 PsychoLama

#include "rectangle.hpp"
#include <QColor>

template<>
int Rect::tBase::s_instances = 0;

Rect::Rect(QObject *parent) : Geometry(parent) {
	setObjectName(tr("Rectangle %1").arg(Rect::tBase::s_instances));
	colors({
		QColor(255, 0, 0),
		QColor(0, 0, 255)
	});
}

template<>
QOpenGLVertexArrayObject* Rect::tBase::s_vao = nullptr;
template<>
ProgramList Rect::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Rect::tBase::s_buffers = {};

template<>
QVector<quint32> Rect::tBase::s_indexBuffer = {
	0, 1, 2,		// Face 1
	2, 3, 0,		//
	6, 5, 4,		// Face 2
	4, 7, 6,		//
};

template<>
QHash<QString, QVector<GLfloat>> Rect::tBase::s_buffersData = {
        {"Position", {
			-1.0f, -1.0f, 0.0f,		// Face 1
			 1.0f, -1.0f, 0.0f,		//
			 1.0f,  1.0f, 0.0f,		//
			-1.0f,  1.0f, 0.0f,		//
			-1.0f, -1.0f, 0.0f,		// Face 2
			 1.0f, -1.0f, 0.0f,		//
			 1.0f,  1.0f, 0.0f,		//
			-1.0f,  1.0f, 0.0f,		//
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
