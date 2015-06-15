// Copyright 2015 PsychoLama

#include <cylinder.hpp>
#include <QtMath>
#include <vector>

// Compteur d'instances
template<>
int Cylinder::tBase::s_instances = 0;

Cylinder::Cylinder(QObject* parent) : Geometry(parent) {
	// Initialisation du nom de l'objet
	setObjectName(QString(tr("Cylinder %1")).arg(Cylinder::tBase::s_instances));

	// Couleurs du cylindre
	colors({
		QColor(255, 0, 0),
		QColor(0, 255, 0),
		QColor(0, 0, 255)
	});
}

// VAO, shaders et buffers
template<>
QOpenGLVertexArrayObject* Cylinder::tBase::s_vao = nullptr;
template<>
ProgramList Cylinder::tBase::s_programList = ProgramList();
template<>
QHash<QString, QOpenGLBuffer*> Cylinder::tBase::s_buffers = {};

// Nombres de cotés du cylindre
int sides = 48;

QVector<GLfloat> calcCylinderVertices() {
	QVector<GLfloat> vertices;
	vertices.reserve(sides* 4 * 3);

	// Le cylindre est généré a partir d'un angle incrémenté a chaque face
	qreal theta = 0;
	for (int j = 0; j < sides; j++) {
		// Tous les vertices sont en double pour créér les cercles du dessus et du dessous du cylindre
		for(int i = 0; i < 2; i++) {
			vertices.append(qCos(theta));
			vertices.append(1.0f);
			vertices.append(qSin(theta));

			vertices.append(qCos(theta));
			vertices.append(-1.0f);
			vertices.append(qSin(theta));
		}

		theta += (2 * M_PI) / sides;
	}

	return vertices;
}

QVector<quint32> calcCylinderIndices() {
	QVector<quint32> indices;

	for (int j = 0; j < sides * 4; j += 4) {
		int mod = sides * 4;
		// 2 triangles forment un quad sur le coté du cylindre
		indices.append((j + 0) % mod);
		indices.append((j + 5) % mod);
		indices.append((j + 1) % mod);

		indices.append((j + 0) % mod);
		indices.append((j + 4) % mod);
		indices.append((j + 5) % mod);

		if(j > 0) {
			// Puis 2 autres triangles sont ajoutés en haut et en bas
			indices.append((j + 2) % mod);
			indices.append(2);
			indices.append((j + 6) % mod);

			indices.append((j + 7) % mod);
			indices.append(3);
			indices.append((j + 3) % mod);
		}
	}

	return indices;
}

QVector<GLfloat> calcCylinderColors() {
	QVector<GLfloat> colors;
	colors.reserve(sides * 4);

	for (int j = 0; j < sides * 4; j++) {
		// La couleur est a 0 pour les 2 premiers vertices, puis a 1 et 2 pour les autres
		colors.append(qMax(0, (j % 4) - 1));
	}

	return colors;
}

QVector<GLfloat> calcCylinderUVs() {
	QVector<GLfloat> UVs;
	UVs.reserve(sides * 4 * 2);

	qreal theta = 0;
	for (int j = 0; j < sides; j++) {
		// Les UV du coté suivent le cylindre
		for(int i = 0; i < 2; i++) {
			UVs.append(i);
			UVs.append(static_cast<float>(j) / sides);
		}

		// Ceux du dessus et du dessous sont calculés comme dans la classe Cercle
		UVs.append(qCos(theta));
		UVs.append(qSin(theta));

		UVs.append(qCos(theta));
		UVs.append(qSin(theta));

		theta += (2 * M_PI) / sides;
	}

	return UVs;
}

template<>
QVector<quint32> Cylinder::tBase::s_indexBuffer = calcCylinderIndices();

template<>
QHash<QString, QVector<GLfloat>> Cylinder::tBase::s_buffersData = {
	{"Position", calcCylinderVertices()},
	{"Color", calcCylinderColors()},
	{"UV", calcCylinderUVs()}
};
