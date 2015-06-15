// Copyright 2015 PsychoLama

#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <fileloader.hpp>
#include <QMainWindow>
#include <QTreeWidgetItem>
#include <QPluginLoader>
#include <QJsonArray>
#include <QCheckBox>
#include <QComboBox>
#include <functional>
#include <tuple>

typedef std::function<QVariant(void)> VarGetter;
typedef std::function<void(const QVariant&)> VarSetter;

namespace Ui {
	class MainWindow;
}

/*! \brief Fenetre principale
 *
 * Charge l'interface de la fenetre principale et en gère les evenements.
 */
class MainWindow : public QMainWindow {
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();
	Ui::MainWindow* getUI() { return ui; }

private slots:
	void on_actionOpen_triggered();
	void on_actionSaveAs_triggered();
	void on_actionSave_triggered();
	void on_viewport_childAdded(QObject *obj);
	void on_showBuffers_toggled(bool arg1);
	void showProperties(QObject* obj);
	void on_actionWorldProp_triggered();
	void on_showMaps_toggled(bool arg1);
	void on_actorList_customContextMenuRequested(const QPoint &pos);
	void on_actorList_itemSelectionChanged();
	void on_actionGroup_triggered();
	void on_actionNew_triggered();

	void loadPlugins();
	void updateTabs();
	void updateTree();

private:
	//! Ajoute un objet a l'arbre des acteurs
	QTreeWidgetItem* addToTree(QObject* obj, QTreeWidgetItem* parent = nullptr);

	//! Recupère un objet depuis une ligne de l'arbre
	QObject* getObject(QTreeWidgetItem* item);

	//! Crée un widget a partir d'une QVariant
	QWidget* widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set);

	//! Tuple réprésentant un Plugin, contenant un pointeur vers le plugin ainsi que ses méta-données
	typedef std::tuple<QObject*, QJsonObject> Plugin;

	QString m_lastFile;
	Ui::MainWindow* ui;
	QList<Plugin> plugins;
	QStringList formats;
	QMap<QString, FileLoader*> loaders;
};

#endif  // TESTMAIN_H
