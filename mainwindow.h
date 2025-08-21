#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include "Task.h"

class TaskWidget;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:

    void onNewTaskButton();
    void onAddTaskButton();
    void onDateClick(const QDate &date);

private:
    Ui::MainWindow *ui;

    QVector<Task> tasks;
    const QString errorStyle = "background-color: #5a1f1f;";
    enum State { default_view, new_task } state = State::default_view;
    QString defaultFileName = "tasks.json";
    QDate pickedDate;

    void updateDefaultView();
    void clearInputWindow();
    void changeState(State state);

};


























#endif // MAINWINDOW_H
