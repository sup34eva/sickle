// Copyright 2015 PsychoLama

#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <QMainWindow>
#include <QTreeWidgetItem>
#include <functional>

typedef std::function<QVariant(void)> VarGetter;
typedef std::function<void(const QVariant&)> VarSetter;

namespace Ui {
	class MainWindow;
}

/*! \class MainWindow
 * \brief Fenetre principale
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
	void on_actorList_currentItemChanged(QTreeWidgetItem *current);
	void on_actionOpen_triggered();
	void on_actionSave_as_triggered();
	void on_action_Save_triggered();
	void on_actionWireframe_toggled(bool arg1);
	void on_viewport_childAdded(QObject *obj);

	void on_newGeo_triggered();

private:
	QWidget* widgetForVariant(QTreeWidgetItem* line, VarGetter get, VarSetter set);
	QString m_lastFile;
	Ui::MainWindow* ui;
};

#endif  // TESTMAIN_H
