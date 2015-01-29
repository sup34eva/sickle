// Copyright 2015 PsychoLama

#include <mainwindow.hpp>
#include <QVariant>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QPushButton>
#include <QList>
#include "./ui_mainwindow.h"

#define M_PI 3.14159265358979323846

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	connect(ui->viewport->camera(), &Camera::moved, [=] (const QVector3D& position) {
		ui->camPos->setText(QString("X: %1, Y: %2, Z: %3").arg(position.x()).arg(position.y()).arg(position.z()));
	});
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_viewport_childAdded(QObject* obj) {
	auto item = new QTreeWidgetItem;
	item->setText(0, obj->objectName());

	auto ptr = QVariant::fromValue(obj);
	item->setData(0, Qt::UserRole, ptr);

	connect(obj, &QObject::objectNameChanged, [=](QString newName) {
		item->setText(0, newName);
	});

	connect(obj, &QObject::destroyed, [=]() {
		auto index = ui->actorList->indexOfTopLevelItem(item);
		delete ui->actorList->takeTopLevelItem(index);
	});

	ui->actorList->addTopLevelItem(item);
}

QQuaternion fromEuler(const QVector3D& euler) {
	auto value = QQuaternion::fromAxisAndAngle(QVector3D(1, 0, 0), euler.x());
	value *= QQuaternion::fromAxisAndAngle(QVector3D(0, 1, 0), euler.y());
	value *= QQuaternion::fromAxisAndAngle(QVector3D(0, 0, 1), euler.z());
	value.normalize();
	return value;
}

QVector3D* toEuler(const QQuaternion& quat) {
	float heading, attitude, bank;
	auto q1 = quat.toVector4D();
	Q_ASSERT(q1.x() == quat.vector().x());
	Q_ASSERT(q1.y() == quat.vector().y());
	Q_ASSERT(q1.z() == quat.vector().z());
	Q_ASSERT(q1.w() == quat.scalar());
	double test = q1.x() * q1.y() + q1.z() * q1.w();
	if (test > 0.499) {
		heading = 2 * atan2(q1.x(), q1.w());
		attitude = M_PI / 2;
		bank = 0;
	} else if (test < -0.499) {
		heading = -2 * atan2(q1.x(), q1.w());
		attitude = - M_PI / 2;
		bank = 0;
	} else {
		double sqx = q1.x() * q1.x();
		double sqy = q1.y() * q1.y();
		double sqz = q1.z() * q1.z();
		heading = atan2(2 * q1.y() * q1.w() - 2 * q1.x() * q1.z(), 1 - 2 * sqy - 2 * sqz);
		attitude = asin(2 * test);
		bank = atan2(2 * q1.x() * q1.w() - 2 * q1.y() * q1.z(), 1 - 2 * sqx - 2 * sqz);
	}
	return new QVector3D(heading, attitude, bank);
}

QWidget* MainWindow::widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set) {
	auto prop = get();
	switch (prop.type()) {
		case QMetaType::QVector3D: {
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto value = qvariant_cast<QVector3D>(prop);
			auto vector = new QVector3D(value);

			auto container = new QWidget;
			auto hbox = new QHBoxLayout;
			hbox->setMargin(1);
			container->setLayout(hbox);

			auto spinners = new QDoubleSpinBox* [3];
			for (int i = 0; i < 3; i++) {
				spinners[i] = new QDoubleSpinBox(container);
				spinners[i]->setRange(-2147483647, 2147483647);
				switch (i) {
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
				connect(spinners[i], changeSignal, [=](double value) {
					switch (i) {
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
					set(*vector);
				});
			}

			return container;
		}
		case QMetaType::QString: {
			auto textBox = new QLineEdit;
			textBox->setText(prop.toString());
			connect(textBox, &QLineEdit::textEdited, set);
			return textBox;
		}
		case QMetaType::QColor: {
			auto colBtn = new QPushButton;

			connect(colBtn, &QPushButton::clicked, [=] () {
				auto value = qvariant_cast<QColor>(get());
				auto col = QColorDialog::getColor(value, nullptr, tr("Face color"));
				set(col);
				colBtn->setText(QString("rgb(%1, %2, %3)").arg(col.red()).arg(col.green()).arg(col.blue()));
				// colBtn->setStyleSheet(QString("background: %1").arg(col.name()));
			});

			return colBtn;
		}
		case QMetaType::QVariantList: {
			auto value = prop.toList();
			auto list = new QVariantList(value);

			for (int i = 0; i < list->size(); i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, QString("Item %1").arg(i));
				line->addChild(item);

				auto widget = widgetForVariant(item, [=] () {
					return list->at(i);
				}, [=] (const QVariant& val) {
					list->operator[](i) = val;
					set(*list);
				});

				line->treeWidget()->setItemWidget(item, 1, widget);
			}

			break;
		}
		case QMetaType::QQuaternion: {
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto value = qvariant_cast<QQuaternion>(prop);
			auto quat = new QQuaternion(value);
			auto vector = toEuler(*quat);

			auto container = new QWidget;
			auto hbox = new QHBoxLayout;
			hbox->setMargin(1);
			container->setLayout(hbox);

			auto spinners = new QDoubleSpinBox* [3];
			for (int i = 0; i < 3; i++) {
				spinners[i] = new QDoubleSpinBox(container);
				spinners[i]->setRange(-2147483647, 2147483647);
				switch (i) {
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
				connect(spinners[i], changeSignal, [=](double value) {
					switch (i) {
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
					set(fromEuler(*vector));
				});
			}

			return container;
		}
		default:
			qDebug() << prop.type();
			return new QLabel(prop.toString());
	}

	return nullptr;
}

void MainWindow::on_actorList_currentItemChanged(QTreeWidgetItem* current) {
	ui->infoWidget->clear();
	if(current != nullptr) {
		auto ptr = current->data(0, Qt::UserRole);
		if ((!ptr.isNull()) && ptr.isValid() && static_cast<QMetaType::Type>(ptr.type()) == QMetaType::QObjectStar) {
			QObject* obj = qvariant_cast<QObject*>(ptr);
			if (obj != nullptr) {
				auto metaObject = obj->metaObject();
				auto count = metaObject->propertyCount();
				for (int i = 0; i < count; i++) {
					auto prop = metaObject->property(i).name();
					auto line = new QTreeWidgetItem;
					line->setText(0, prop);
					ui->infoWidget->addTopLevelItem(line);
					ui->infoWidget->setItemWidget(line, 1, widgetForVariant(line, [=] () {
						return obj->property(prop);
					}, [=] (const QVariant& val) {
						obj->setProperty(prop, val);
					}));
				}
			}
		}
	}
}

void MainWindow::on_actionOpen_triggered() {
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), tr("World File (*.wld)"));
	ui->viewport->load(fileName);
	m_lastFile = fileName;
}

void MainWindow::on_actionSave_as_triggered() {
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("World File (*.wld)"));
	ui->viewport->save(fileName);
	m_lastFile = fileName;
}

void MainWindow::on_action_Save_triggered() {
	if (m_lastFile.isEmpty())
		on_actionSave_as_triggered();
	else
		ui->viewport->save(m_lastFile);
}

void MainWindow::on_actionWireframe_toggled(bool checked) {
	if (checked) {
		ui->viewport->renderMode(GL_LINES);
	} else {
		ui->viewport->renderMode(GL_TRIANGLES);
	}
}

void MainWindow::on_newGeo_triggered() {
	ui->viewport->addChild<Cube>();
}
