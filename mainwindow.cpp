#include "mainwindow.h"
#include <QMenuBar>
#include <QMenu>
#include <QMessageBox>
#include <QLabel>
#include <QStatusBar>
#include "camera.h"

MainWindow::MainWindow(): m_viewport()
{
    auto menuBar = new QMenuBar;

    auto menuWindow = menuBar->addMenu(tr("&File"));

    auto newGeo = new QAction(menuWindow);
    newGeo->setText(tr("New"));
    connect(newGeo, &QAction::triggered, &m_viewport, &Viewport::addChild);
    menuWindow->addAction(newGeo);

    auto save = new QAction(menuWindow);
    save->setText(tr("Save"));
    connect(save, &QAction::triggered, &m_viewport, &Viewport::save);
    menuWindow->addAction(save);

    auto load = new QAction(menuWindow);
    load->setText(tr("Load"));
    connect(load, &QAction::triggered, &m_viewport, &Viewport::load);
    menuWindow->addAction(load);

    setMenuBar(menuBar);

    setCentralWidget(&m_viewport);

    auto posLabel = new QLabel;
    statusBar()->addPermanentWidget(posLabel);

    auto camera = m_viewport.camera();
    connect(camera, &Camera::moved, [=] (QVector3D val) {
        posLabel->setText(QString("X: %1, Y: %2, Z: %3").arg(val.x()).arg(val.y()).arg(val.z()));
    });
}

MainWindow::~MainWindow() {
    //delete m_viewport;
}
