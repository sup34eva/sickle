// Copyright 2015 PsychoLama

#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_3_Core>
#include <QOpenGLFramebufferObject>
#include <QPainter>
#include <QWheelEvent>
#include <QKeyEvent>
#include <QColor>
#include <QMetaObject>
#include <QtMath>
#include <camera.hpp>
#include <cube.hpp>
#include <sphere.hpp>
#include <world.hpp>

class Light;

/*! \brief Cadre de vue 3D
 *
 * Ce widget affiche une scène en 3D composée de géometries a partir d'une caméra.
 */
class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core {
	Q_OBJECT

public:
	explicit Viewport(QWidget* parent = nullptr);

	QSize minimumSizeHint() const Q_DECL_OVERRIDE {
		return QSize(500, 500);
	}

	QSize sizeHint() const Q_DECL_OVERRIDE {
		return QSize(500, 500);
	}

	//! Ajoute un enfant a la scène
	template<typename T>
	T* addChild() {
		makeCurrent();
		auto child = m_world->currentZone()->addChild<T>();
		doneCurrent();
		emit childAdded(child);
		return child;
	}

	//! Liste les modes de rendus
	QStringList programList() {
		static const QStringList list = {
			tr("Wireframe"),
			tr("Unlit"),
			tr("Light")
		};
		return list;
	}

	propRO(Camera*, camera);
	propSig(bool, isInitialized, initialized);
	prop(bool, showBuffers);
	prop(bool, showMaps);
	prop(QString, program);
	prop(QColor, ambient);
	prop(QColor, bgColor);
	prop(World*, world);

#ifdef UNIT_TEST
	void updateNow() {
		makeCurrent();
		paintGL();
		doneCurrent();
	}
#endif

public slots:
	//! Vide le niveau
	void clearLevel();

	//! Initialise une lumière
	void initLight(Light&);

	//! Mets a jour toutes les lumières
	void updateLights();

	//! Mets a jour une lumière
	void updateLight(Light* light);

	//! Supprimer une lumière de la liste de mise a jours
	void removeLight(Light* light);

	//! Attrape une eventuelle erreur OpenGL
	void catchErrors();

signals:
	//! Lors de l'ajout d'un nouvel enfant
	void childAdded(QObject* child);

	//! Lors de l'ajout d'une nouvelle zone
	void zoneAdded(int index);

protected:
	//! Appelé a l'initialisation du contexte
	void initializeGL() Q_DECL_OVERRIDE;

	//! Appelé a chaque mise a jour du viewport
	void paintGL() Q_DECL_OVERRIDE;

	//! Initialise la scène
	void initScene();

	//! Initialise le quad fullscreen
	void initQuad();

	//! Rends une shadowmap
	void renderLight(Light* light);

	//! Rends une scène
	void renderScene();

	//! Affiche le quad
	void renderQuad();

	//! Appelé lorsque le widget est redimensionné
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;

	//! Appelé lorsque la route de la souris est tournée
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;

	//! Appelé a l'appui d'une touche
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;

	//! Appelé lorsque la souris est appuyée
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	//! Appelée lorsque la souris est relachée
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	//! Appelé lorsque la souris est déplacée
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;

	int heightForWidth(int w) const { return w; }
	bool hasHeightForWidth() const { return true; }

private:
	QMatrix4x4 m_projection;
	QPoint m_cursor;
	QMetaObject::Connection m_relay;

	// Deferred shading
	GLuint m_sceneBuffer;
	QList<GLuint> m_sceneTextures;
	GLuint m_sceneDepth;
	GLuint m_quadVAO;
	GLuint m_quadBuffer;
	QList<QOpenGLShaderProgram*> m_quadPrograms;
	QList<Light*> m_dirtyLights;
};

#endif
