// Copyright 2015 PsychoLama

#include <mainwindow.hpp>
#include <actorbuilder.hpp>
#include <sphere.hpp>
#include <pyramide.hpp>
#include <rectangle.hpp>
#include <trapeze.hpp>
#include <line.hpp>
#include <cylinder.hpp>
#include <light.hpp>
#include <spotlight.hpp>
#include <group.hpp>
#include <trigger.hpp>
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

	auto addTab = ui->tabBar->addTab("+");
	ui->tabBar->setTabButton(addTab, QTabBar::RightSide, nullptr);
	ui->tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);

	connect(ui->tabBar, &QTabBar::currentChanged, [=] (int index) {
		auto zones = ui->viewport->world()->zoneList().size();
		if(index > zones - 1) {
			ui->viewport->world()->addZone();
		} else {
			ui->viewport->world()->setCurrentZoneId(index);
			updateTree();
			ui->viewport->update();
		}
	});

	connect(ui->tabBar, &QTabBar::tabCloseRequested, [=] (int index) {
		if(ui->tabBar->count() > 2) {
			ui->tabBar->removeTab(index);
			ui->viewport->world()->removeZone(index);
		}
	});

	connect(ui->viewport, &Viewport::zoneAdded, [&](int index) {
		ui->tabBar->insertTab(index, tr("Zone %1").arg(index));
		ui->tabBar->setCurrentIndex(index);
	});

	loadPlugins();

	auto modeList = new QComboBox;
	ui->toolBar->insertWidget(ui->actionGroup, modeList);

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
			auto fileName = args.at(1);
			loaders[QFileInfo(fileName).suffix()]->load(ui->viewport, fileName);
		}
	});
}

void MainWindow::loadPlugins() {
	// Plugins par defaut
	QJsonObject fsData, abData;
	fsData["type"] = DefaultFileLoader::tr("Sickle World");
	fsData["extensions"] = QJsonArray({"wld"});

	plugins.append(Plugin(new DefaultFileLoader(), fsData));
	plugins.append(Plugin(new DefaultActorBuilder(), abData));

	auto pluginsDir = QDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
	if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#elif defined(Q_OS_MAC)
	if (pluginsDir.dirName() == "MacOS") {
		pluginsDir.cdUp();
		pluginsDir.cdUp();
		pluginsDir.cdUp();
	}
#endif
	pluginsDir.cd("plugins");

	foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		auto plugin = loader.instance();
		if (plugin)
			plugins.append(Plugin(plugin, loader.metaData()["MetaData"].toObject()));
		else
			qWarning() << loader.errorString();
	}

	QMenu* addMenu = new QMenu(tr("Add Geometry"));
	QAction* geo3D = addMenu->addSeparator(),
			*geo2D = addMenu->addSeparator(),
			*lights = addMenu->addSeparator();

	foreach(Plugin plugin, plugins) {
		auto data = std::get<1>(plugin);

		auto loader = qobject_cast<FileLoader*>(std::get<0>(plugin));
		auto builder = qobject_cast<ActorBuilder*>(std::get<0>(plugin));

		if(loader) {
			QStringList extensions;
			foreach(QJsonValue ext, data["extensions"].toArray())
				extensions.append(ext.toString());

			QString type = data["type"].toString();
			qDebug() << "FileLoader" << data;
			formats.append(QString("%1 (%2)").arg(type).arg(extensions.join(", *.").prepend("*.")));
			foreach(QString suffix, extensions)
				loaders[suffix] = loader;
		} else if (builder) {
			auto classes = builder->getClasses();
			for(int i = 0; i < classes.size(); i++) {
				auto aClass = classes.at(i);

				auto action = new QAction(tr("New %1").arg(std::get<0>(aClass)), addMenu);
				connect(action, &QAction::triggered, [=]() {
					builder->build(ui->viewport, i);
				});

				switch(std::get<1>(aClass)) {
					case ActorBuilder::GEOMETRY_3D:
						addMenu->insertAction(geo3D, action);
						break;
					case ActorBuilder::GEOMETRY_2D:
						addMenu->insertAction(geo2D, action);
						break;
					case ActorBuilder::LIGHT:
						addMenu->insertAction(lights, action);
						break;
					default:
						addMenu->addAction(action);
						break;
				}
			}
		}
	}

	addMenu->menuAction()->setIcon(QIcon(":/icons/add-geo.png"));
	ui->toolBar->insertAction(ui->actionGroup, addMenu->menuAction());
}

MainWindow::~MainWindow() {
	delete ui;
}

QTreeWidgetItem* MainWindow::addToTree(QObject* obj, QTreeWidgetItem* parent) {
	auto item = new QTreeWidgetItem;
	item->setText(0, obj->objectName());

	auto ptr = QVariant::fromValue(obj);
	item->setData(0, Qt::UserRole, ptr);

	connect(obj, &QObject::objectNameChanged, [=](QString newName) { item->setText(0, newName); });

	connect(obj, &QObject::destroyed, this, &MainWindow::updateTree);

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

	ui->viewport->updateLights();
	ui->viewport->update();
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
					item->setText(0, tr(name));
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
			line->setText(0, tr(prop));
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

void MainWindow::updateTabs() {
	auto zones = ui->viewport->world()->zoneList();
	auto count = ui->tabBar->count() - 1;

	ui->tabBar->setCurrentIndex(0);

	if(count < zones.size()) {
		for(int i = count; i < zones.size(); i++) {
			ui->tabBar->insertTab(i - 1, tr("Zone %1").arg(i));
		}
	} else if (count > zones.size()) {
		for(int i = count; i > zones.size(); i--) {
			ui->tabBar->removeTab(i - 1);
		}
	}

	ui->tabBar->setCurrentIndex(ui->viewport->world()->currentZoneId());
}

void MainWindow::updateTree() {
	ui->actorList->clear();
	auto zone = ui->viewport->world()->currentZone();
	foreach(auto i, zone->children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) addToTree(child);
	}
}

void MainWindow::on_actionOpen_triggered() {
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), formats.join(";;"));
	auto info = QFileInfo(fileName);
	if(info.exists()) {
		loaders[info.suffix()]->load(ui->viewport, fileName);
		m_lastFile = fileName;

		updateTabs();
		updateTree();
		ui->viewport->updateLights();
		ui->viewport->update();
	}
}

void MainWindow::on_actionSaveAs_triggered() {
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), formats.join(";;"));
	if(!fileName.isEmpty()) {
		loaders[QFileInfo(fileName).suffix()]->save(ui->viewport, fileName);
		m_lastFile = fileName;
	}
}

void MainWindow::on_actionSave_triggered() {
	if (m_lastFile.isEmpty())
		on_actionSaveAs_triggered();
	else
		loaders[QFileInfo(m_lastFile).suffix()]->save(ui->viewport, m_lastFile);
}

void MainWindow::on_showBuffers_toggled(bool show) {
	ui->viewport->showBuffers(show);
	ui->viewport->update();
}

void MainWindow::on_actionWorldProp_triggered() {
	showProperties(ui->viewport->world());
}

void MainWindow::on_showMaps_toggled(bool show) {
	ui->viewport->showMaps(show);
	ui->viewport->update();
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
		ui->viewport->updateLights();
		ui->viewport->update();
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

void MainWindow::on_actionNew_triggered() {
	ui->viewport->clearLevel();
	updateTabs();
	updateTree();
	ui->viewport->updateLights();
	ui->viewport->update();
}
