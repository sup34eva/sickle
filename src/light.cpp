// Copyright 2015 PsychoLama

#include <light.hpp>
#include <viewport.hpp>

Light::Light(QObject* parent) : Actor(parent) {
	// Les paramètres par défaut sont assez large pour couvrir toute la zone par défaut, au détriment de la qualité des ombres
	nearZ(-63);
	farZ(66);
	mapSize(QSize(140, 103));

	// La lumière a de base une puissance de 1 et un lumière blanche
	power(1.0);
	color(QColor(255, 255, 255));

	// Orientation a 45deg pour une ombre inclinée
	orientation(fromEuler(QVector3D(-45, 0, -45)));

	// Compteur d'instances pour le nom de l'objet
	static int instances = 1;
	setObjectName(tr("Light %1").arg(instances++));

	// La lumière se met a jour lorsque ses propriétés sont modifiées
	connect(this, &Light::nearZChanged, this, &Light::update);
	connect(this, &Light::farZChanged, this, &Light::update);
	connect(this, &Light::mapSizeChanged, this, &Light::update);
	connect(this, &Light::moved, this, &Light::update);
	connect(this, &Light::rotated, this, &Light::update);

	// Mise a jour initiale pour calculer les ombres une première fois
	update();
	if(parent != nullptr)
		setParent(parent);
}

Light::~Light() {
	// Avant sa suppresion, la lumière se retire de la liste des lumières modifiées du viewport pour eviter les segfault
	auto view = viewport();
	if(view != nullptr) view->removeLight(this);
}

void Light::updateProj() {
	m_projection.setToIdentity();
	auto size = m_mapSize / 2;
	m_projection.ortho(-size.width(), size.width(), -size.height(), size.height(), m_nearZ, m_farZ);
}

void Light::updateView() {
	m_view.setToIdentity();
	m_view.lookAt(direction(), QVector3D(0, 0, 0), QVector3D(0, 1, 0));
}

void Light::update() {
	auto view = viewport();

	// Mise a jour des matrices
	updateProj();
	updateView();

	// Mise a jour de la shadowmap
	if(view != nullptr)
		view->updateLight(this);
}

void Light::setParent(QObject* parent) {
	Actor::setParent(parent);
	auto view = viewport();
	if(view != nullptr) {
		// La lumière demande aui viewport de l'initialiser
		view->initLight(*this);

		// Puis calcule une premère fois sa shadowmap
		view->updateLight(this);
	}
}

QVector3D Light::direction() {
	auto dir = orientation().toVector4D();
	return QVector3D(    2 * (dir.x() * dir.z() + dir.w() * dir.y()),
						 2 * (dir.y() * dir.x() - dir.w() * dir.x()),
					 1 - 2 * (dir.x() * dir.x() + dir.y() * dir.y())).normalized();
}

QMatrix4x4 Light::depth() {
	return m_projection * m_view;
}
