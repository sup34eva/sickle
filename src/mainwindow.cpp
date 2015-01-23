#include <mainwindow.hpp>
#include "ui_mainwindow.h"
#include <QVariant>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::on_centralwidget_childAdded(QObject* obj) {
    auto item = new QListWidgetItem(obj->objectName(), ui->listWidget);
    auto ptr = QVariant::fromValue(obj);
    item->setData(Qt::UserRole, ptr);
    connect(obj, &QObject::objectNameChanged, [=] (QString newName) {
        item->setText(newName);
    });
    connect(obj, &QObject::destroyed, [=] () {
        ui->listWidget->removeItemWidget(item);
        delete item;
    });
}

QQuaternion fromEuler(const QVector3D& euler) {
    double c1 = cos(euler.x()/2);
    double s1 = sin(euler.x()/2);
    double c2 = cos(euler.y()/2);
    double s2 = sin(euler.y()/2);
    double c3 = cos(euler.z()/2);
    double s3 = sin(euler.z()/2);
    double c1c2 = c1 * c2;
    double s1s2 = s1 * s2;
    auto w = c1c2 * c3 - s1s2 * s3;
    auto x = c1c2 * s3 + s1s2 * c3;
    auto y = s1 * c2 * c3 + c1 * s2 * s3;
    auto z = c1 * s2 * c3 - s1 * c2 * s3;
    auto angle = 2 * acos(w);
    double norm = x * x + y * y + z * z;
    if (norm < 0.001) {
        x = 1;
        y = z = 0;
    } else {
        norm = sqrt(norm);
        x /= norm;
        y /= norm;
        z /= norm;
    }
    return QQuaternion(angle, x, y, z);
}

QWidget* MainWindow::widgetForVariant(QObject* obj, const char* name) {
    auto prop = obj->property(name);
    switch(prop.type()) {
        case QMetaType::QVector3D: {
            void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
            auto value = qvariant_cast<QVector3D>(prop);
            auto vector = new QVector3D(value);

            auto container = new QWidget;
            auto hbox = new QHBoxLayout;
            container->setLayout(hbox);

            auto spinners = new QDoubleSpinBox*[3];
            for(int i = 0; i < 3; i++) {
                spinners[i] = new QDoubleSpinBox(container);
                spinners[i]->setRange(-2147483647, 2147483647);
                switch(i) {
                    case 0:
                        spinners[i]->setValue(vector->x());
                        break;
                    case 1:
                        spinners[i]->setValue(vector->y());
                        break;
                    case 2:
                        spinners[i]->setValue(vector->z());
                        break;
                }
                hbox->addWidget(spinners[i]);
                connect(spinners[i], changeSignal, [=] (double value) {
                    switch(i) {
                        case 0:
                            vector->setX(value);
                            break;
                        case 1:
                            vector->setY(value);
                            break;
                        case 2:
                            vector->setZ(value);
                            break;
                    }
                    obj->setProperty(name, *vector);
                });
            }

            return container;
        }
        case QMetaType::QString: {
            auto textBox = new QLineEdit;
            textBox->setText(prop.toString());
            connect(textBox, &QLineEdit::textEdited, [=] (const QString& text) {
                obj->setProperty(name, text);
            });
            return textBox;
        }
        /*case QMetaType::QQuaternion: {
            //TODO
        }*/
        default:
            qDebug() << prop.type();
            return new QLabel(prop.toString());
    }
}

void MainWindow::on_listWidget_currentItemChanged(QListWidgetItem *current) {
    auto ptr = current->data(Qt::UserRole);
    if(ptr.isValid() && static_cast<QMetaType::Type>(ptr.type()) == QMetaType::QObjectStar) {
        QObject* obj = qvariant_cast<QObject*>(ptr);
        if(obj) {
            auto info = ui->infoWidget;
            auto metaObject = obj->metaObject();
            auto count = metaObject->propertyCount();
            info->clear();
            info->setColumnCount(2);
            info->setRowCount(count);
            for(int i = 0; i < count; ++i) {
                auto prop = metaObject->property(i).name();
                info->setItem(i, 0, new QTableWidgetItem(prop));
                info->setCellWidget(i, 1, widgetForVariant(obj, prop));
            }
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), tr("World File (*.wld)"));
    ui->centralwidget->load(fileName);
    m_lastFile = fileName;
}

void MainWindow::on_actionSave_as_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("World File (*.wld)"));
    ui->centralwidget->save(fileName);
    m_lastFile = fileName;
}

void MainWindow::on_action_Save_triggered()
{
    if(m_lastFile.isEmpty())
        on_actionSave_as_triggered();
    else
        ui->centralwidget->save(m_lastFile);
}

void MainWindow::on_actionWireframe_toggled(bool checked)
{
    if(checked) {
        ui->centralwidget->renderMode(GL_LINES);
    } else {
        ui->centralwidget->renderMode(GL_TRIANGLES);
    }
}
