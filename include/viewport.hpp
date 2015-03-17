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
#include <camera.hpp>
#include <cube.hpp>
#include <sphere.hpp>

class Light;

/*! \brief Cadre de vue 3D
 *
 * Ce widget affiche une scène en 3D composée de géometries a partir d'une caméra.
 */
class Viewport : public QOpenGLWidget, protected QOpenGLFunctions_4_3_Core {
	Q_OBJECT

public:
	explicit Viewport(QWidget* parent = nullptr);
	~Viewport();

	QSize minimumSizeHint() const Q_DECL_OVERRIDE {
		return QSize(500, 500);
	}

	QSize sizeHint() const Q_DECL_OVERRIDE {
		return QSize(500, 500);
	}

	template<typename T>
	T* addChild() {
		makeCurrent();
		auto child = new T(this);
		doneCurrent();
		emit childAdded(child);
		return child;
	}

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

#ifdef UNIT_TEST
	void updateNow() {
		makeCurrent();
		paintGL();
		doneCurrent();
	}
#endif

public slots:
	void save(QString name);
	void load(QString name);
	void clearLevel();
	void initLight(Light&);
	void updateLights();
	void updateLight(Light* light);

signals:
	void childAdded(QObject* child);

protected:
	void initializeGL() Q_DECL_OVERRIDE;
	void paintGL() Q_DECL_OVERRIDE;
	void initScene();
	void initQuad();
	void renderLight(Light*);
	void renderScene();
	void renderQuad();
	void resizeGL(int w, int h) Q_DECL_OVERRIDE;
	void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
	void keyPressEvent(QKeyEvent* event) Q_DECL_OVERRIDE;
	void mousePressEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseReleaseEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	void mouseMoveEvent(QMouseEvent* event) Q_DECL_OVERRIDE;
	int heightForWidth(int w) const { return w; }
	bool hasHeightForWidth() const { return true; }

private:
	QMatrix4x4 m_projection;
	QPoint m_cursor;

	// Deferred shading
	GLuint m_sceneBuffer;
	QList<GLuint> m_sceneTextures;
	GLuint m_sceneDepth;
	GLuint m_quadVAO;
	GLuint m_quadBuffer;
	QList<QOpenGLShaderProgram*> m_quadPrograms;
	QList<Light*> m_dirtyLights;
};

QDataStream& operator<<(QDataStream&, const QObject&);
QDataStream& operator>>(QDataStream&, QObject&);

#endif
