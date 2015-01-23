#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <QOpenGLFunctions>
#include <QWheelEvent>
#include <QKeyEvent>
#include <camera.h>
#include <geometry.h>
#include <cube.h>

/*! \class Viewport
 * \brief Cadre de vue 3D
 *
 * Ce widget affiche une scène en 3D composée de géometries a partir d'une caméra.
 */
class Viewport : public QOpenGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    Viewport(QWidget *parent = nullptr);
    ~Viewport();

    QSize minimumSizeHint() const Q_DECL_OVERRIDE;
    QSize sizeHint() const Q_DECL_OVERRIDE;
    propRO(Camera*, camera)
    prop(GLenum, renderMode)

public slots:
    Geometry* addChild() {
        makeCurrent();
        auto child = new Cube(this);
        doneCurrent();
        child->setObjectName("testObj");
        child->setParent(this);
        emit childAdded(child);
        return child;
    }
    void save(QString name);
    void load(QString name);
    void clearLevel();

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

QDataStream &operator<<(QDataStream &, const QObject &);
QDataStream &operator>>(QDataStream &, QObject &);

#endif
