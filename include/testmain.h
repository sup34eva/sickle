#ifndef TESTMAIN_H
#define TESTMAIN_H

#include <QMainWindow>
#include <QListWidgetItem>

namespace Ui {
class TestMain;
}

class TestMain : public QMainWindow
{
    Q_OBJECT

public:
    explicit TestMain(QWidget *parent = 0);
    ~TestMain();

private slots:
    void on_centralwidget_childAdded(QObject *obj);

    void on_listWidget_currentItemChanged(QListWidgetItem *current);

    void on_actionOpen_triggered();

    void on_actionSave_as_triggered();

    void on_action_Save_triggered();

private:
    Ui::TestMain *ui;
};

#endif // TESTMAIN_H
