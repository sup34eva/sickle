#include <testmain.h>
#include "ui_testmain.h"
#include <QVariant>

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

QTableWidgetItem* widgetForVariant(QVariant prop) {
    switch(prop.type()) {
        case QMetaType::QVector3D: {
            auto vector = qvariant_cast<QVector3D>(prop);
            return new QTableWidgetItem(QString("X: %1, Y: %2, Z: %3").arg(vector.x()).arg(vector.y()).arg(vector.z()));
        }
        case QMetaType::QQuaternion: {
            auto quat = qvariant_cast<QQuaternion>(prop);
            return new QTableWidgetItem(QString("Pitch: %1, Yaw: %2, Roll: %3").arg(quat.x()).arg(quat.y()).arg(quat.z()));
        }
        default:
            qDebug() << prop.type();
            return new QTableWidgetItem(prop.toString());
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
                info->setItem(i - offset, 1, widgetForVariant(obj->property(prop)));
            }
        }
    }
}
