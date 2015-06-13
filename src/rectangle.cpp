// Copyright 2015 PsychoLama

#include "rectangle.hpp"
#include <QColor>

template<>
int Rect::tBase::s_instances = 0;

Rect::Rect(QObject *parent) : Geometry(parent) {
	setObjectName(QString(tr("Rectangle %1")).arg(Rect::tBase::s_instances));
	colors({
		QColor(255, 0, 0)
	});
}

void Rect::draw(const DrawInfo &info) {
	auto func = info.context->functions();
	func->glDisable(GL_CULL_FACE);
	tBase::draw(info);
	func->glEnable(GL_CULL_FACE);
}

template<>
QOpenGLVertexArrayObject* Rect::tBase::s_vao = nullptr;
template<>
ProgramList Rect::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Rect::tBase::s_buffers = {};

template<>
QVector<quint32> Rect::tBase::s_indexBuffer = {
	0,  1,  2,		// Face 1
	2,  3,  0,		// Face 2
};

template<>
QHash<QString, QVector<GLfloat>> Rect::tBase::s_buffersData = {
        {"Position", {
			-1.0f, -1.0f, 0.0f,		// Face 1
			 1.0f, -1.0f, 0.0f,		//
			 1.0f,  1.0f, 0.0f,		//
			-1.0f,  1.0f, 0.0f,		//
        }},
        {"Color", {
			0.0f,		// Face 1
			0.0f,		//
			0.0f,		//
			0.0f,		//
        }},
        {"UV", {
			0.0f,  0.0f,		// Face 1
			0.0f,  1.0f,		//
			1.0f,  0.0f,		//
			1.0f,  1.0f,		//
        }}
};
