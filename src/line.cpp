// Copyright 2015 PsychoLama

#include <line.hpp>

template<>
int Line::tBase::s_instances = 0;

Line::Line(QObject* parent) : Geometry(parent), m_point1(0, 0, 0), m_point2(1, 1, 1) {
	// Initialise le nom de la ligne avec le compteur d'instances
	setObjectName(QString(tr("Line %1")).arg(Line::tBase::s_instances));

	// Initialise la liste des couleurs
	colors({
		QColor(255, 0, 0),
		QColor(0, 255, 0)
	});
}

void Line::draw(const DrawInfo &info) {
	// Force l'affichage de la ligne en mode fil de fer
	auto lineInfo = info;
	lineInfo.mode = GL_LINES;
	tBase::draw(lineInfo);
}

QMatrix4x4 Line::transform() {
	QMatrix4x4 transform;

	// Translate la ligne jusqu'a son premier point de controle
	transform.translate(m_point1);

	// Puis l'étire pour atteindre le deuxième point
	transform.scale(m_point2 - m_point1);

	// Ajoute la transformation de base de l'Acteur
	transform *= Actor::transform();
	return transform;
}

// VAO, shaders et buffers
template<>
QOpenGLVertexArrayObject* Line::tBase::s_vao = nullptr;
template<>
ProgramList Line::tBase::s_programList = {};
template<>
QHash<QString, QOpenGLBuffer*> Line::tBase::s_buffers = {};

// La ligne est composé d'un triangle reliant 2 points
template<>
QVector<quint32> Line::tBase::s_indexBuffer = {
	0, 1, 0
};

template<>
QHash<QString, QVector<GLfloat>> Line::tBase::s_buffersData = {
	{"Position", {
		0.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f,
	}},
	{"Color", {
		 0.0f,
		 1.0f
	 }},
	{"UV", {
		 0.0f, 0.0f,
		 1.0f, 1.0f
	 }}
};

