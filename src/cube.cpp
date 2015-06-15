// Copyright 2015 PsychoLama

#include <cube.hpp>
#include <vector>

//
template<>
int Cube::tBase::s_instances = 0;

Cube::Cube(QObject* parent) : Geometry(parent) {
	// Définit le nom du cube
	setObjectName(QString(tr("Cube %1")).arg(Cube::tBase::s_instances));

	// Initialisation de la liste des couleurs
	colors({
		QColor(255,	0,		0),
		QColor(255,	255, 	0),
		QColor(0,	255, 	0),
		QColor(0,	255, 	255),
		QColor(0,	0,		255),
		QColor(255,	0,		255)
	});
}

QVector<GLfloat> calcCubeUVs() {
	QVector<GLfloat> UVs;
	UVs.reserve(6 * 4 * 2);

	for (int face = 0; face < 6; face++) {
		// L'UV map est la même pour toutes les faces
		UVs.append(0.0f); UVs.append(0.0f);
		UVs.append(0.0f); UVs.append(1.0f);
		UVs.append(1.0f); UVs.append(1.0f);
		UVs.append(1.0f); UVs.append(0.0f);
	}

	return UVs;
}

QVector<GLfloat> calcCubeColors() {
	QVector<GLfloat> colors;
	colors.reserve(6 * 4);

	for (GLfloat face = 0; face < 6; face++) for (int vert = 0; vert < 4; vert++) {
		// L'ID de la couleur est simplement celui de la face
		colors.append(face);
	}

	return colors;
}

QVector<quint32> calcCubeIndices() {
	QVector<quint32> indices;
	indices.reserve(6 * 2 * 3);

	for (GLfloat face = 0; face < 6; face++) {
		// Les indices sont constants pour chaque face
		GLfloat f = face * 4;
		indices.append(f + 0);
		indices.append(f + 1);
		indices.append(f + 3);

		indices.append(f + 3);
		indices.append(f + 1);
		indices.append(f + 2);
	}
	return indices;
}

// Initialisation des VAO, shaders et buffers
template<>
QOpenGLVertexArrayObject* Cube::tBase::s_vao = nullptr;
template<>
ProgramList Cube::tBase::s_programList = {};
template<>
QHash<QString, QOpenGLBuffer*> Cube::tBase::s_buffers = {};

// Buffers
template<>
QVector<quint32> Cube::tBase::s_indexBuffer = calcCubeIndices();

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
	{"Color", calcCubeColors()},
	{"UV", calcCubeUVs()}
};
