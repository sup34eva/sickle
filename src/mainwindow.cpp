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
#include <QtMath>
#include "./ui_mainwindow.h"

QString toString(const QVector3D& vector) {
	return QString("X: %1, Y: %2, Z: %3").arg(vector.x()).arg(vector.y()).arg(vector.z());
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);
	connect(ui->viewport->camera(), &Camera::moved, [=] (const QVector3D& position) {
		ui->camPos->setText(toString(position));
	});

	connect(ui->viewport, &Viewport::initialized, [=] () {
		auto args = QCoreApplication::arguments();
		if(args.length() > 1) {
			ui->viewport->load(args.at(1));
		}
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
	qreal pitch, yaw, roll;
	const auto q = quat.toVector4D();
	const auto w2 = q.w() * q.w();
	const auto x2 = q.x() * q.x();
	const auto y2 = q.y() * q.y();
	const auto z2 = q.z() * q.z();
	const auto unitLength = w2 + x2 + y2 + z2;
	const auto abcd = q.w() * q.x() + q.y() * q.z();
	const auto eps = 1e-7;
	if (abcd > (0.5 - eps) * unitLength) {
		yaw = 2 * qAtan2(q.y(), q.w());
		pitch = M_PI;
		roll = 0;
	} else if (abcd < (-0.5 + eps) * unitLength) {
		yaw = -2 * qAtan2(q.y(), q.w());
		pitch = -M_PI;
		roll = 0;
	} else {
		const auto adbc = q.w() * q.z() - q.x() * q.y();
		const auto acbd = q.w() * q.y() - q.x() * q.z();
		yaw = qAtan2(2 * adbc, 1 - 2 * (z2 + x2));
		pitch = qAsin(2 * abcd / unitLength);
		roll = qAtan2(2 * acbd, 1 - 2 * (y2 + x2));
	}
	return new QVector3D(qRadiansToDegrees(pitch), qRadiansToDegrees(yaw), qRadiansToDegrees(roll));
}

QWidget* MainWindow::widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set) {
	auto prop = get();
	switch (prop.type()) {
		case QMetaType::QVector3D: {
			auto value = qvariant_cast<QVector3D>(prop);
			auto vector = new QVector3D(value);
			auto label = new QLabel(toString(*vector));

			for (int i = 0; i < 3; i++) {
				auto item = new QTreeWidgetItem;
				switch (i) {
					case 0:
						item->setText(0, QString("X"));
						break;
					case 1:
						item->setText(0, QString("Y"));
						break;
					case 2:
						item->setText(0, QString("Z"));
						break;
				}
				line->addChild(item);

				auto widget = widgetForVariant(item, [=] () {
					switch (i) {
						case 0:
							return vector->x();
						case 1:
							return vector->y();
						case 2:
							return vector->z();
					}
				}, [=] (const QVariant& val) {
					switch (i) {
						case 0:
							vector->setX(val.toFloat());
							break;
						case 1:
							vector->setY(val.toFloat());
							break;
						case 2:
							vector->setZ(val.toFloat());
							break;
					}
					label->setText(toString(*vector));
					set(*vector);
				});

				line->treeWidget()->setItemWidget(item, 1, widget);
			}

			return label;
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
			auto value = qvariant_cast<QQuaternion>(prop);
			auto quat = new QQuaternion(value);
			auto vector = toEuler(*quat);
			auto label = new QLabel(toString(*vector));

			for (int i = 0; i < 3; i++) {
				auto item = new QTreeWidgetItem;
				switch (i) {
					case 0:
						item->setText(0, QString("Pitch"));
						break;
					case 1:
						item->setText(0, QString("Yaw"));
						break;
					case 2:
						item->setText(0, QString("Roll"));
						break;
				}
				line->addChild(item);

				auto widget = widgetForVariant(item, [=] () {
					switch (i) {
						case 0:
							return vector->x();
						case 1:
							return vector->y();
						case 2:
							return vector->z();
					}
				}, [=] (const QVariant& val) {
					switch (i) {
						case 0:
							vector->setX(val.toFloat());
							break;
						case 1:
							vector->setY(val.toFloat());
							break;
						case 2:
							vector->setZ(val.toFloat());
							break;
					}
					label->setText(toString(*vector));
					set(fromEuler(*vector));
				});

				line->treeWidget()->setItemWidget(item, 1, widget);
			}

			return label;
		}
		case QMetaType::Float: {
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto spinner = new QDoubleSpinBox;
			spinner->setRange(-2147483647, 2147483647);
			spinner->setValue(prop.toFloat());
			connect(spinner, changeSignal, [=](double value) {
				set(value);
			});
			return spinner;
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
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), tr("Sickle World (*.wld)"));
	ui->viewport->load(fileName);
	m_lastFile = fileName;
}

void MainWindow::on_actionSave_as_triggered() {
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), tr("Sickle World (*.wld)"));
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
