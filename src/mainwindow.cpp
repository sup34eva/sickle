// Copyright 2015 PsychoLama

#include <mainwindow.hpp>
#include <sphere.hpp>
#include <light.hpp>
#include <spotlight.hpp>
#include <group.hpp>
#include <QVariant>
#include <QFileDialog>
#include <QSpinBox>
#include <QLabel>
#include <QLineEdit>
#include <QColorDialog>
#include <QPushButton>
#include <QList>
#include <QtMath>
#include <limits>
#include "./ui_mainwindow.h"

QString toString(const QVector3D& vector) {
	return QString(MainWindow::tr("X: %1, Y: %2, Z: %3")).arg(vector.x()).arg(vector.y()).arg(vector.z());
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	ui->setupUi(this);

	ui->tabBar->addTab("+");
	connect(ui->tabBar, &QTabBar::currentChanged, [=] (int index) {
		if(index == ui->tabBar->count() - 1) {
			ui->viewport->world()->addZone();
		} else {
			ui->viewport->world()->setCurrentZone(index);

			auto zone = ui->viewport->world()->currentZone();
			ui->actorList->clear();
			foreach(auto i, zone->children()) {
				auto child = dynamic_cast<Actor*>(i);
				if (child) addToTree(child);
			}

			ui->viewport->update();
		}
	});

	connect(ui->viewport->world(), &World::zoneAdded, [&](int index) {
		ui->tabBar->insertTab(index, tr("Zone %1").arg(index));
		ui->tabBar->setCurrentIndex(index);
	});

	QMenu* addMenu = new QMenu(tr("Add Geometry"));
	addMenu->addAction(ui->newCube);
	addMenu->addAction(ui->newSphere);
	addMenu->addAction(ui->newLight);
	addMenu->addAction(ui->newSpot);
	addMenu->menuAction()->setIcon(QIcon(":/icons/add-geo.png"));
	ui->toolBar->addAction(addMenu->menuAction());

	auto modeList = new QComboBox;
	ui->toolBar->addWidget(modeList);

	ui->toolBar->addAction(ui->actionGroup);

	ui->camPos->setText(toString(QVector3D(0, 0, 0)));
	connect(ui->viewport->camera(), &Camera::moved,
			[=](const QVector3D& position) { ui->camPos->setText(toString(position)); });

	connect(ui->viewport, &Viewport::initialized, [=]() {
		auto list = ui->viewport->programList();
		for(auto i = list.constBegin(); i != list.constEnd(); ++i) {
			modeList->insertItem(i - list.constBegin(), *i);
		}
		modeList->setCurrentText(ui->viewport->program());
		connect(modeList, &QComboBox::currentTextChanged, [=] (const QString& index) {
			ui->viewport->program(index);
			ui->viewport->update();
		});

		auto args = QCoreApplication::arguments();
		if (args.length() > 1) {
			ui->viewport->load(args.at(1));
		}
	});
}

QTreeWidgetItem* MainWindow::addToTree(QObject* obj, QTreeWidgetItem* parent) {
	auto item = new QTreeWidgetItem;
	item->setText(0, obj->objectName());

	auto ptr = QVariant::fromValue(obj);
	item->setData(0, Qt::UserRole, ptr);

	connect(obj, &QObject::objectNameChanged, [=](QString newName) { item->setText(0, newName); });

	connect(obj, &QObject::destroyed, [=]() {
		if(parent == nullptr) {
			auto index = ui->actorList->indexOfTopLevelItem(item);
			delete ui->actorList->takeTopLevelItem(index);
		} else {
			auto index = parent->indexOfChild(item);
			parent->takeChild(index);
		}

		ui->viewport->update();
	});

	foreach(auto i, obj->children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) addToTree(child, item);
	}

	if(parent == nullptr)
		ui->actorList->addTopLevelItem(item);
	else
		parent->addChild(item);

	return item;
}

MainWindow::~MainWindow() {
	delete ui;
}

void MainWindow::on_viewport_childAdded(QObject* obj) {
	auto item = addToTree(obj);
	auto group = qobject_cast<Group*>(obj);
	if(group != nullptr) {
		auto selection = ui->actorList->selectedItems();
		foreach(auto itm, selection) {
			auto obj = getObject(itm);
			obj->setParent(group);

			auto parent = itm->parent();
			if(parent != nullptr) {
				parent->removeChild(itm);
			} else {
				auto index = ui->actorList->indexOfTopLevelItem(itm);
				ui->actorList->takeTopLevelItem(index);
			}

			item->addChild(itm);
		}
	}

	ui->viewport->update();
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
	const auto eps = std::numeric_limits<float>::epsilon();
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
	if(!prop.isValid())
		return nullptr;
	switch (static_cast<QMetaType::Type>(prop.type())) {
		case QMetaType::QVector3D: {
			auto value = qvariant_cast<QVector3D>(prop);
			auto vector = new QVector3D(value);
			auto label = new QLabel(toString(*vector));
			const QString axis[] = {"X", "Y", "Z"};

			for (int i = 0; i < 3; i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, axis[i]);
				line->addChild(item);

				auto widget = widgetForVariant(item, [=]() {
					return (*vector)[i];
				}, [=](const QVariant& val) {
					(*vector)[i] = val.toFloat();
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
			static int btnCount = 0;
			auto colBtn = new QPushButton;
			colBtn->setObjectName(QString("colBtn-%1").arg(btnCount++));

			auto initValue = qvariant_cast<QColor>(get());
			auto dialog = new QColorDialog(initValue, colBtn);
			dialog->setOptions(QColorDialog::NoButtons);

			QString style("#%1 { border-image: none; border-radius: 5px; background-color: %2;}");
			auto updateBtn = [=](const QColor& color) {
				colBtn->setStyleSheet(style.arg(colBtn->objectName(), color.name()));
				colBtn->update();
			};

			connect(dialog, &QColorDialog::currentColorChanged, [=](const QColor& color) {
				if(color.isValid()) {
					set(color);
					updateBtn(color);
				}
			});

			connect(colBtn, &QPushButton::clicked, [=]() {
				auto currentValue = qvariant_cast<QColor>(get());
				dialog->setCurrentColor(currentValue);
				dialog->open();
			});

			updateBtn(initValue);

			return colBtn;
		}
		case QMetaType::QVariantList: {
			auto value = prop.toList();
			auto list = new QVariantList(value);

			for (int i = 0; i < list->size(); i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, QString("Item %1").arg(i));
				line->addChild(item);

				auto widget = widgetForVariant(item, [=]() {
					return list->at(i);
				}, [=](const QVariant& val) {
					list->replace(i, val);
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
			const QString axis[] = {"Pitch", "Yaw", "Roll"};

			for (int i = 0; i < 3; i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, axis[i]);
				line->addChild(item);

				auto widget = widgetForVariant(item, [=]() {
					return (*vector)[i];
				}, [=](const QVariant& val) {
					(*vector)[i] = val.toFloat();
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
			auto limit = std::numeric_limits<float>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toFloat());
			connect(spinner, changeSignal, [=](double value) { set(value); });
			return spinner;
		}
		case QMetaType::Double: {
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto spinner = new QDoubleSpinBox;
			auto limit = std::numeric_limits<double>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toDouble());
			connect(spinner, changeSignal, [=](double value) { set(value); });
			return spinner;
		}
		case QMetaType::Int: {
			void (QSpinBox::*changeSignal)(int) = &QSpinBox::valueChanged;
			auto spinner = new QSpinBox;
			auto limit = std::numeric_limits<int>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toInt());
			connect(spinner, changeSignal, [=](int value) { set(value); });
			return spinner;
		}
		case QMetaType::QObjectStar: {
			auto obj = qvariant_cast<QObject*>(prop);
			if (obj != nullptr) {
				auto metaObject = obj->metaObject();
				auto count = metaObject->propertyCount();
				for (int i = metaObject->propertyOffset(); i < count; i++) {
					auto name = metaObject->property(i).name();

					auto item = new QTreeWidgetItem;
					item->setText(0, name);
					line->addChild(item);

					QWidget* widget;
					if(metaObject->property(i).isEnumType()) {
						auto enumerator = metaObject->property(i).enumerator();
						auto cb = new QComboBox;

						for(int i = 0; i < enumerator.keyCount(); i++) {
							cb->insertItem(i, enumerator.key(i));
						}

						cb->setCurrentIndex(obj->property(name).toInt());

						void (QComboBox::*changeSignal)(int) = &QComboBox::currentIndexChanged;
						connect(cb, changeSignal, [=] (int index) {
							obj->setProperty(name, index);
							ui->viewport->update();
						});

						widget = cb;
					} else {
						widget = widgetForVariant(item, [=]() {
							return obj->property(name);
						}, [=](const QVariant& val) {
							obj->setProperty(name, val);
							ui->viewport->update();
						});
					}

					line->treeWidget()->setItemWidget(item, 1, widget);
				}

				return new QLabel(obj->objectName());
			}

			break;
		}
		case QMetaType::Bool: {
			auto checkbox = new QCheckBox;
			checkbox->setChecked(prop.toBool());
			connect(checkbox, &QCheckBox::toggled, [=] (bool checked) { set(checked); });
			return checkbox;
		}
		case QMetaType::QSize: {
			QSize* value = new QSize(prop.toSize());
			auto label = new QLabel(prop.toString());
			const QString axis[] = {
				tr("Width"),
				tr("Height")
			};

			for (int i = 0; i < 2; i++) {
				auto item = new QTreeWidgetItem;
				item->setText(0, axis[i]);
				line->addChild(item);

				auto widget = widgetForVariant(item, [=]() {
					if(i == 0)
						return value->width();
					else
						return value->height();
				}, [=](const QVariant& val) {
					if(i == 0)
						value->setWidth(val.toInt());
					else
						value->setHeight(val.toInt());
					set(*value);
				});

				line->treeWidget()->setItemWidget(item, 1, widget);
			}

			return label;
		}
		default:
			qDebug() << "Unknown property type:" << prop.type();
		case QMetaType::QRect:
		case QMetaType::QPoint:
		case QMetaType::QRegion:
		case QMetaType::QSizePolicy:
		case QMetaType::QPalette:
		case QMetaType::QFont:
		case QMetaType::QCursor:
		case QMetaType::QIcon:
		case QMetaType::QLocale:
		case QMetaType::QMatrix4x4:
			return new QLabel(prop.toString());
	}

	return nullptr;
}

void MainWindow::showProperties(QObject* obj) {
	ui->infoWidget->clear();
	if(obj != nullptr) {
		auto metaObject = obj->metaObject();
		auto count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			auto prop = metaObject->property(i).name();
			auto line = new QTreeWidgetItem;
			line->setText(0, prop);
			ui->infoWidget->addTopLevelItem(line);
			QWidget* widget;

			if(metaObject->property(i).isEnumType()) {
				auto enumerator = metaObject->property(i).enumerator();
				auto cb = new QComboBox;

				for(int i = 0; i < enumerator.keyCount(); i++) {
					cb->insertItem(i, enumerator.key(i));
				}

				cb->setCurrentIndex(obj->property(prop).toInt());

				void (QComboBox::*changeSignal)(int) = &QComboBox::currentIndexChanged;
				connect(cb, changeSignal, [=] (int index) {
					obj->setProperty(prop, index);
					ui->viewport->update();
				});

				widget = cb;
			} else {
				widget = widgetForVariant(line, [=]() {
					return obj->property(prop);
				}, [=](const QVariant& val) {
					obj->setProperty(prop, val);
					ui->viewport->update();
				});
			}

			ui->infoWidget->setItemWidget(line, 1, widget);
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

void MainWindow::on_newCube_triggered() {
	ui->viewport->addChild<Cube>();
}

void MainWindow::on_newSphere_triggered() {
	ui->viewport->addChild<Sphere>();
}

void MainWindow::on_actionBuffers_toggled(bool show) {
	ui->viewport->showBuffers(show);
	ui->viewport->update();
}

void MainWindow::on_actionSceneProp_triggered() {
	showProperties(ui->viewport);
}

void MainWindow::on_newLight_triggered() {
	ui->viewport->addChild<Light>();
}

void MainWindow::on_showMaps_toggled(bool show) {
	ui->viewport->showMaps(show);
	ui->viewport->update();
}

void MainWindow::on_newSpot_triggered() {
	ui->viewport->addChild<Spotlight>();
}

QObject* MainWindow::getObject(QTreeWidgetItem* item) {
	if (item != nullptr) {
		auto ptr = item->data(0, Qt::UserRole);
		if ((!ptr.isNull()) && ptr.isValid() && static_cast<QMetaType::Type>(ptr.type()) == QMetaType::QObjectStar) {
			return qvariant_cast<QObject*>(ptr);
		}
	}
	return nullptr;
}

void MainWindow::on_actorList_customContextMenuRequested(const QPoint& pos) {
	auto item = ui->actorList->itemAt(pos);
	if(ui->actorList->selectedItems().indexOf(item) == -1)
		ui->actorList->setCurrentItem(item);

	QMenu menu(ui->actorList);

	auto del = new QAction(tr("&Delete"), ui->actorList);
	del->setStatusTip(tr("Delete this actor"));

	connect(del, &QAction::triggered, [=]() {
		auto selection = ui->actorList->selectedItems();
		foreach(auto itm, selection) {
			auto obj = getObject(itm);
			if(ui->actorList->currentItem() == itm)
				ui->infoWidget->clear();
			obj->deleteLater();
		}
	});

	menu.addAction(del);

	menu.exec(ui->actorList->mapToGlobal(pos));
}

void MainWindow::on_actorList_itemSelectionChanged() {
	if(ui->actorList->selectedItems().length() == 1)
		showProperties(getObject(ui->actorList->currentItem()));
	else
		ui->infoWidget->clear();
}

void MainWindow::on_actionGroup_triggered() {
	ui->viewport->addChild<Group>();
}
