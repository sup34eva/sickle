#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QKeyEvent>
#include "camera.h"
#include "geometry.h"

class Viewport : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Viewport(QWidget *parent = nullptr);
    ~Viewport();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    propRO(Camera*, camera)

public slots:
    Geometry* addChild() {
        makeCurrent();
        auto child = new Geometry(this);
        doneCurrent();
        child->setObjectName("testObj");
        child->setParent(this);
        emit childAdded(child);
        return child;
    }
    void save(QString name);
    void load(QString name);

signals:
    void childAdded(QObject* child);

protected:
    void initializeGL() Q_DECL_OVERRIDE;
    void paintGL() Q_DECL_OVERRIDE;
    void resizeGL(int w, int h) Q_DECL_OVERRIDE;
    void wheelEvent(QWheelEvent* event) Q_DECL_OVERRIDE;
    void keyPressEvent (QKeyEvent* event) Q_DECL_OVERRIDE;
    void mousePressEvent (QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent (QMouseEvent* event) Q_DECL_OVERRIDE;
    void mouseMoveEvent (QMouseEvent* event) Q_DECL_OVERRIDE;
    int heightForWidth(int w) {return w;}
    bool hasHeightForWidth() {return true;}

private:
    QMatrix4x4 m_projection;
    QPoint m_cursor;
};

#endif
