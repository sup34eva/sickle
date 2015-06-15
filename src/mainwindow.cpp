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

//! Convertit un vecteur en string
QString toString(const QVector3D& vector) {
	return QString(MainWindow::tr("X: %1, Y: %2, Z: %3")).arg(vector.x()).arg(vector.y()).arg(vector.z());
}

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
	// Charge l'interface depuis le designer
	ui->setupUi(this);

	// Ajoute un onglet special permettant d'ajouter des zones
	auto addTab = ui->tabBar->addTab("+");

	// Supprime le bouton de fermeture de l'onglet
	ui->tabBar->setTabButton(addTab, QTabBar::RightSide, nullptr);

	// Lorsque l'utilisateur ferme un onglet, passe sur l'onglet de gauche
	ui->tabBar->setSelectionBehaviorOnRemove(QTabBar::SelectLeftTab);

	// Evenement appelé lorsque l'utilisateur change d'onglet
	connect(ui->tabBar, &QTabBar::currentChanged, [=] (int index) {
		auto zones = ui->viewport->world()->zoneList().size();
		if(index > zones - 1) {
			// Si le dernier onglet a été ouvert, ajoute une zone
			ui->viewport->world()->addZone();
		} else {
			// Sinon, change de zone
			ui->viewport->world()->setCurrentZoneId(index);

			// Met a jour l'arborescence de la scène
			updateTree();

			// Et met a jour l'image du viewport
			ui->viewport->update();
		}
	});

	// Lorsque l'utilisateur ferme un onglet
	connect(ui->tabBar, &QTabBar::tabCloseRequested, [=] (int index) {
		// Si il y a au moins 2 zones
		if(ui->tabBar->count() > 2) {
			// Supprimer l'onglet
			ui->tabBar->removeTab(index);

			// Et supprime la zone correspondante
			ui->viewport->world()->removeZone(index);
		}
	});

	// Lors de l'ajout d'un zone
	connect(ui->viewport, &Viewport::zoneAdded, [&](int index) {
		// Insère un nouvel onglet
		ui->tabBar->insertTab(index, tr("Zone %1").arg(index));

		// Passe sur l'onglet novellement créé
		ui->tabBar->setCurrentIndex(index);
	});

	// Charge les plugins
	loadPlugins();

	// ajoute la liste des modes de rendus a la toolbar
	auto modeList = new QComboBox;
	ui->toolBar->insertWidget(ui->actionGroup, modeList);

	// Initialise la statusbar et enregistre un evenement lorsque la caméra bouge
	ui->camPos->setText(toString(QVector3D(0, 0, 0)));
	connect(ui->viewport->camera(), &Camera::moved,
			[=](const QVector3D& position) { ui->camPos->setText(toString(position)); });

	// Lorsque le viewport s'initialise
	connect(ui->viewport, &Viewport::initialized, [=]() {
		// Liste les modes de rendu
		auto list = ui->viewport->programList();
		for(auto i = list.constBegin(); i != list.constEnd(); ++i) {
			// Pour chacun d'eux, ajoute une ligne au menu
			modeList->insertItem(i - list.constBegin(), *i);
		}

		// Change la valeur du menu pour le mode courant
		modeList->setCurrentText(ui->viewport->program());

		// Lorsque l'utilisateur change de mode
		connect(modeList, &QComboBox::currentTextChanged, [=] (const QString& index) {
			// Change de programme
			ui->viewport->program(index);

			// Et met a jour le viewport
			ui->viewport->update();
		});

		// Lit les arguments de l'application pour charger les fichiers ouverts avec celle-ci
		auto args = QCoreApplication::arguments();
		if (args.length() > 1) {
			auto fileName = args.at(1);
			// Extrait l'extension du fichier et le charge avec le FileLoader approprié
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

	// Charge le dossier des plugins
	auto pluginsDir = QDir(qApp->applicationDirPath());
	while(pluginsDir.entryList(QDir::AllDirs).indexOf("plugins") == -1)
		pluginsDir.cdUp();
	pluginsDir.cd("plugins");

	// Pour chaque fichier du dossier plugins
	foreach(QString fileName, pluginsDir.entryList(QDir::Files)) {
		// Essaie de le charger en tant que plugin
		QPluginLoader loader(pluginsDir.absoluteFilePath(fileName));
		auto plugin = loader.instance();
		if (plugin)  // Si il n'y a pas eu d'erreurs, le plugins et ses métadonnées sont ajoutés a la liste
			plugins.append(Plugin(plugin, loader.metaData()["MetaData"].toObject()));
		else
			qWarning() << loader.errorString();
	}

	// Crée le menu des géométries et ses séparateurs
	QMenu* addMenu = new QMenu(tr("Add Geometry"));
	QAction* geo3D = addMenu->addSeparator(),
			*geo2D = addMenu->addSeparator(),
			*lights = addMenu->addSeparator();

	// Pour chaque plugin
	foreach(Plugin plugin, plugins) {
		auto data = std::get<1>(plugin);

		// Caste le plugin dans toutes les interfaces existantes
		auto loader = qobject_cast<FileLoader*>(std::get<0>(plugin));
		auto builder = qobject_cast<ActorBuilder*>(std::get<0>(plugin));

		// Si le plugin est un FileLoader
		if(loader) {
			// Liste les extensions prises en charge par ce plugin
			QStringList extensions;
			foreach(QJsonValue ext, data["extensions"].toArray())
				extensions.append(ext.toString());

			// Crée le format de ce chargeur
			QString type = data["type"].toString();
			formats.append(QString("%1 (%2)").arg(type).arg(extensions.join(", *.").prepend("*.")));

			// Ajoute le FileLoader a la liste des chargeurs
			foreach(QString suffix, extensions)
				loaders[suffix] = loader;
		} else if (builder) {  // Si le plugin est un ActorBuilder
			// Lit la liste des classes de ce builder
			auto classes = builder->getClasses();

			// Pour chaque classe
			for(int i = 0; i < classes.size(); i++) {
				auto aClass = classes.at(i);

				// Crée une action permettant de créér l'acteur correspondant
				auto action = new QAction(tr("New %1").arg(std::get<0>(aClass)), addMenu);
				connect(action, &QAction::triggered, [=]() {
					builder->build(ui->viewport, i);
				});

				// Insère l'action dans le menu en fonction de sa catégorie
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

	// Ajoute le menu a la toolbar
	addMenu->menuAction()->setIcon(QIcon(":/icons/add-geo.png"));
	ui->toolBar->insertAction(ui->actionGroup, addMenu->menuAction());
}

MainWindow::~MainWindow() {
	delete ui;
}

QTreeWidgetItem* MainWindow::addToTree(QObject* obj, QTreeWidgetItem* parent) {
	// Crée une ligne pour l'objet
	auto item = new QTreeWidgetItem;
	item->setText(0, obj->objectName());

	// Enregistre le pointeur de l'objet dans la ligne
	auto ptr = QVariant::fromValue(obj);
	item->setData(0, Qt::UserRole, ptr);

	// Change le texte de la ligne lorsque le nom de l'objet change
	connect(obj, &QObject::objectNameChanged, [=](QString newName) { item->setText(0, newName); });

	// Met a jour l'arbre lorsque l'objet est supprimé
	connect(obj, &QObject::destroyed, this, &MainWindow::updateTree);

	// Pour chaque enfant de l'acteur
	foreach(auto i, obj->children()) {
		// Le cast en acteur
		auto child = dynamic_cast<Actor*>(i);

		// Et l'ajoute a l'arbre
		if (child) addToTree(child, item);
	}

	// Si l'objet n'a pas de parent, l'ajoute a la liste des acteurs
	if(parent == nullptr)
		ui->actorList->addTopLevelItem(item);
	else  // Sinon, l'ajoute a la ligne de l'acteur parent
		parent->addChild(item);

	return item;
}

void MainWindow::on_viewport_childAdded(QObject* obj) {
	// Ajoute le nouvel item a l'arbre
	auto item = addToTree(obj);

	// Si l'objet est un groupe
	auto group = qobject_cast<Group*>(obj);
	if(group != nullptr) {
		// Liste les acteurs séléctionnés
		auto selection = ui->actorList->selectedItems();

		// Pour chaque element de la séléction
		foreach(auto itm, selection) {
			// Récupère l'objet depuis sa ligne
			auto obj = getObject(itm);

			// Change son objet parent pour le groupe
			obj->setParent(group);

			// Si la ligne avait un parent
			auto parent = itm->parent();
			if(parent != nullptr) {
				// Retire la ligne a son parent
				parent->removeChild(itm);
			} else {  // Sinon, retire l'objet du widget
				auto index = ui->actorList->indexOfTopLevelItem(itm);
				ui->actorList->takeTopLevelItem(index);
			}

			// Ajoute le ligne a celle du groupe
			item->addChild(itm);
		}
	}

	// Met a jour le viewport
	ui->viewport->updateLights();
	ui->viewport->update();
}

QWidget* MainWindow::widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set) {
	// Récupère la valeurs
	auto prop = get();
	if(!prop.isValid())
		return nullptr;
	switch (static_cast<QMetaType::Type>(prop.type())) {
		case QMetaType::QVector3D: {
			// Pour un vecteur, affiche 3 sous-champs spinner piour entrer les composantes du vecteur
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
			// Pour un string, affiche simplement un champ de texte
			auto textBox = new QLineEdit;
			textBox->setText(prop.toString());
			connect(textBox, &QLineEdit::textEdited, set);
			return textBox;
		}
		case QMetaType::QColor: {
			// Pour une couleur, affiche un bouton customisé pour afficher la couleur
			// Lorsqu'il est activé, le bouton affiche une boite de dialogue permettant de choisir une couleur
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
			// Pour une liste, affiche une sous-ligne pour chaque valeur
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
			// Pour un quaternion, le principe est le même que pour un vecteur mais le nom des champs est différent
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
			// Pour un float, affiche un doubleSpinner re-casté en float
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto spinner = new QDoubleSpinBox;
			auto limit = std::numeric_limits<float>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toFloat());
			connect(spinner, changeSignal, [=](double value) { set(value); });
			return spinner;
		}
		case QMetaType::Double: {
			// Pour un double, utilise simplement un doubleSpinner
			void (QDoubleSpinBox::*changeSignal)(double) = &QDoubleSpinBox::valueChanged;
			auto spinner = new QDoubleSpinBox;
			auto limit = std::numeric_limits<double>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toDouble());
			connect(spinner, changeSignal, [=](double value) { set(value); });
			return spinner;
		}
		case QMetaType::Int: {
			// Pour un int, utilise une SpinBox
			void (QSpinBox::*changeSignal)(int) = &QSpinBox::valueChanged;
			auto spinner = new QSpinBox;
			auto limit = std::numeric_limits<int>::max();
			spinner->setRange(-limit, limit);
			spinner->setValue(prop.toInt());
			connect(spinner, changeSignal, [=](int value) { set(value); });
			return spinner;
		}
		case QMetaType::QObjectStar: {
			// Pour un pointeur vers un qobject, affiche une sous ligne pour chaque propriété
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
						// Pour les propriétés de type enum, affiche une QComboBox
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
			// Pour les valeur booléennes, affiche une checkbox
			auto checkbox = new QCheckBox;
			checkbox->setChecked(prop.toBool());
			connect(checkbox, &QCheckBox::toggled, [=] (bool checked) { set(checked); });
			return checkbox;
		}
		case QMetaType::QSize: {
			// Pour une taille, le principe est le même que pour un vecteur mais en 2 dimensions
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
		// Sinon, affiche un label et tente de convertir la valeur en string
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
	// Vide la fenetre d'informations
	ui->infoWidget->clear();

	if(obj != nullptr) {
		// Récupère le meta-objet de l'objet pour lister les propriétés
		auto metaObject = obj->metaObject();
		auto count = metaObject->propertyCount();
		for (int i = 0; i < count; i++) {
			// Crée une ligne par propriété
			auto prop = metaObject->property(i).name();
			auto line = new QTreeWidgetItem;
			line->setText(0, tr(prop));
			ui->infoWidget->addTopLevelItem(line);
			QWidget* widget;

			// Si la valeur est un type enum
			if(metaObject->property(i).isEnumType()) {
				// Affiche une comboBox
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
				// Sinon utilise widgetForVariant
				widget = widgetForVariant(line, [=]() {
					return obj->property(prop);
				}, [=](const QVariant& val) {
					obj->setProperty(prop, val);
					ui->viewport->update();
				});
			}

			// Ajout le widget a la ligne
			ui->infoWidget->setItemWidget(line, 1, widget);
		}
	}
}

void MainWindow::updateTabs() {
	// Liste les zones
	auto zones = ui->viewport->world()->zoneList();
	auto count = ui->tabBar->count() - 1;

	// Passe sur l'onglet 0
	ui->tabBar->setCurrentIndex(0);

	// Supprime ou ajoute des onglets selon la nécéssité
	if(count < zones.size()) {
		for(int i = count; i < zones.size(); i++) {
			ui->tabBar->insertTab(i - 1, tr("Zone %1").arg(i));
		}
	} else if (count > zones.size()) {
		for(int i = count; i > zones.size(); i--) {
			ui->tabBar->removeTab(i - 1);
		}
	}

	// Repasse sur l'onglet de la zone courante
	ui->tabBar->setCurrentIndex(ui->viewport->world()->currentZoneId());
}

void MainWindow::updateTree() {
	// Vide l'arbre des acteurs
	ui->actorList->clear();

	// Recupère la zone courante
	auto zone = ui->viewport->world()->currentZone();

	// Ajoute tous les enfant directs a l'arbre
	foreach(auto i, zone->children()) {
		auto child = dynamic_cast<Actor*>(i);
		if (child) addToTree(child);
	}
}

void MainWindow::on_actionOpen_triggered() {
	// Affiche une boite de dialogue pour l'ouverture d'un fichier
	auto fileName = QFileDialog::getOpenFileName(this, tr("Open World"), QString(), formats.join(";;"));

	// Séléctionne le chargeur a utiliser en fonction de l'extension du fichier choisi
	auto info = QFileInfo(fileName);
	if(info.exists()) {
		loaders[info.suffix()]->load(ui->viewport, fileName);
		m_lastFile = fileName;

		// Après le chargement, met a jour toute la fenetre
		updateTabs();
		updateTree();
		ui->viewport->updateLights();
		ui->viewport->update();
	}
}

void MainWindow::on_actionSaveAs_triggered() {
	// Affiche une boite de dialogue pour la sauvegarde d'un fichier
	auto fileName = QFileDialog::getSaveFileName(this, tr("Save World"), QString(), formats.join(";;"));

	// Si un fichier a été séléctionné
	if(!fileName.isEmpty()) {
		// L'enregistre
		loaders[QFileInfo(fileName).suffix()]->save(ui->viewport, fileName);

		// Et mémorise son nom
		m_lastFile = fileName;
	}
}

void MainWindow::on_actionSave_triggered() {
	// Si aucun fichier n'est en mémoire, ouvre l'action "Enregistrer sous"
	if (m_lastFile.isEmpty())
		on_actionSaveAs_triggered();
	else  // Sinon, ecrase le fichier en mémoire
		loaders[QFileInfo(m_lastFile).suffix()]->save(ui->viewport, m_lastFile);
}

void MainWindow::on_showBuffers_toggled(bool show) {
	// Affiche les buffers
	ui->viewport->showBuffers(show);

	// Et met a jour le viewport
	ui->viewport->update();
}

void MainWindow::on_actionWorldProp_triggered() {
	// Affiche les propriétés du monde
	showProperties(ui->viewport->world());
}

void MainWindow::on_showMaps_toggled(bool show) {
	// Affiche les shadowmaps
	ui->viewport->showMaps(show);

	// Et met a jour le viewport
	ui->viewport->update();
}

QObject* MainWindow::getObject(QTreeWidgetItem* item) {
	// Si l'item existe
	if (item != nullptr) {
		// Récupère les données
		auto ptr = item->data(0, Qt::UserRole);

		// Vérifie leur validité
		if ((!ptr.isNull()) && ptr.isValid() && static_cast<QMetaType::Type>(ptr.type()) == QMetaType::QObjectStar) {
			// Récupère le pointeur
			return qvariant_cast<QObject*>(ptr);
		}
	}
	return nullptr;
}

void MainWindow::on_actorList_customContextMenuRequested(const QPoint& pos) {
	// Trouve la ligne cliquée
	auto item = ui->actorList->itemAt(pos);

	// La séléctionne si elle ne l'est pas
	if(ui->actorList->selectedItems().indexOf(item) == -1)
		ui->actorList->setCurrentItem(item);

	// Crée le menu
	QMenu menu(ui->actorList);

	// Crée l'action de suppression
	auto del = new QAction(tr("&Delete"), ui->actorList);
	del->setStatusTip(tr("Delete this actor"));

	// Supprime la selection lorsque l'action est activée
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

	// Ajoute l'action au menu
	menu.addAction(del);

	// Affiche le menu
	menu.exec(ui->actorList->mapToGlobal(pos));
}

void MainWindow::on_actorList_itemSelectionChanged() {
	// N'affiche des informations sur l'acteur séléctionné que si une seule ligne est surlignée
	if(ui->actorList->selectedItems().length() == 1)
		showProperties(getObject(ui->actorList->currentItem()));
	else
		ui->infoWidget->clear();
}

void MainWindow::on_actionGroup_triggered() {
	// Ajoute un groupe
	ui->viewport->addChild<Group>();
}

void MainWindow::on_actionNew_triggered() {
	// Vide le niveau
	ui->viewport->clearLevel();

	// Met a jour l'interface
	updateTabs();
	updateTree();

	// Met a jour le viewport
	ui->viewport->updateLights();
	ui->viewport->update();
}
