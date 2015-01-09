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
    void save() {
        QFile file("file.dat");
        file.open(QIODevice::WriteOnly);
        QDataStream out(&file);
        out << static_cast<quint32>(0xB00B1E5); // Magic number

        auto version = QDataStream::Qt_5_4; // Format version
        out << static_cast<qint32>(version);
        out.setVersion(version);

        // Data
        out << *camera();
        auto childList = findChildren<Geometry*>();
        out << static_cast<quint32>(childList.size());
        for(auto obj : childList) {
            out << *obj;
        }
    }
    void load() {
        QFile file("file.dat");
        file.open(QIODevice::ReadOnly);
        QDataStream in(&file);
        quint32 magic;
        in >> magic; // Magic number

        qint32 version; // Format version
        in >> version;
        in.setVersion(version);

        // Data
        in >> *camera();
        quint32 size;
        in >> size;
        qDebug() << size;
        for(quint32 i = 0; i < size; i++) {
            Geometry* obj = addChild();
            in >> *obj;
        }
    }

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
