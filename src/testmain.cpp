#include <testmain.h>
#include "ui_testmain.h"
#include <QVariant>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>

TestMain::TestMain(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::TestMain)
{
    ui->setupUi(this);
}

TestMain::~TestMain() {
    delete ui;
}

void TestMain::on_centralwidget_childAdded(QObject* obj) {
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

QWidget* TestMain::widgetForVariant(QObject* obj, const char* name) {
    auto prop = obj->property(name);
    switch(prop.type()) {
        case QMetaType::QVector3D: {
            void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
            auto value = qvariant_cast<QVector3D>(prop);
            auto vector = new QVector3D(value);
            auto container = new QWidget;
            auto hbox = new QHBoxLayout;
            container->setLayout(hbox);

            auto spinboxX = new QDoubleSpinBox(container);
            spinboxX->setValue(vector->x());
            spinboxX->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxX);
            connect(spinboxX, changeSignal, [=] (double value) {
                vector->setX(value);
                obj->setProperty(name, *vector);
            });

            auto spinboxY = new QDoubleSpinBox(container);
            spinboxY->setValue(vector->y());
            spinboxY->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxY);
            connect(spinboxY, changeSignal, [=] (double value) {
                vector->setY(value);
                obj->setProperty(name, *vector);
            });

            auto spinboxZ = new QDoubleSpinBox(container);
            spinboxZ->setValue(vector->z());
            spinboxZ->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxZ);
            connect(spinboxZ, changeSignal, [=] (double value) {
                vector->setZ(value);
                obj->setProperty(name, *vector);
            });

            return container;
        }
        case QMetaType::QQuaternion: {
            void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
            auto value = qvariant_cast<QQuaternion>(prop);
            auto quat = new QQuaternion(value);
            QQuaternion pitch, yaw, roll;

            auto container = new QWidget;
            auto hbox = new QHBoxLayout;
            container->setLayout(hbox);

            auto spinboxX = new QDoubleSpinBox(container);
            spinboxX->setValue(quat->vector().x());
            spinboxX->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxX);
            connect(spinboxX, changeSignal, [=] (double value) {
                pitch = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), value);
                obj->setProperty(name, pitch * yaw * roll);
            });

            auto spinboxY = new QDoubleSpinBox(container);
            spinboxY->setValue(quat->vector().y());
            spinboxY->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxY);
            connect(spinboxY, changeSignal, [=] (double value) {
                yaw = QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), value);
                obj->setProperty(name, pitch * yaw * roll);
            });

            auto spinboxZ = new QDoubleSpinBox(container);
            spinboxZ->setValue(quat->vector().z());
            spinboxZ->setRange(-2147483647, 2147483647);
            hbox->addWidget(spinboxZ);
            connect(spinboxZ, changeSignal, [=] (double value) {
                roll = QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), value);
                obj->setProperty(name, pitch * yaw * roll);
            });

            return container;
        }
        default:
            qDebug() << prop.type();
            return new QLabel(prop.toString());
    }
}

void TestMain::on_listWidget_currentItemChanged(QListWidgetItem *current) {
    auto ptr = current->data(Qt::UserRole);
    if(ptr.isValid() && ptr.type() == QMetaType::QObjectStar) {
        QObject* obj = qvariant_cast<QObject*>(ptr);
        if(obj) {
            auto info = ui->infoWidget;
            auto metaObject = obj->metaObject();
            auto count = metaObject->propertyCount();
            auto offset = metaObject->propertyOffset();
            info->clear();
            info->setColumnCount(2);
            info->setRowCount(count - offset);
            for(int i = offset; i < count; ++i) {
                auto prop = metaObject->property(i).name();
                info->setItem(i - offset, 0, new QTableWidgetItem(prop));
                info->setCellWidget(i - offset, 1, widgetForVariant(obj, prop));
            }
        }
    }
}

void TestMain::on_actionOpen_triggered()
{
    auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), tr("World File (*.wld)"));
    ui->centralwidget->load(fileName);
}

void TestMain::on_actionSave_as_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("World File (*.wld)"));
    ui->centralwidget->save(fileName);
}

void TestMain::on_action_Save_triggered()
{
    auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("World File (*.wld)"));
    ui->centralwidget->save(fileName);
}
