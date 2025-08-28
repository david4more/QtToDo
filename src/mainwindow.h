#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTime>
#include <QFile>
#include <QSaveFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>
#include <QTextCharFormat>
#include <QToolButton>
#include <QColorDialog>
#include <QInputDialog>
#include <QDialog>
#include <QLabel>
#include <QCheckBox>

class Task;
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
    void updateTasksFile();
    void onPickColorButton();
    void onDateClick(const QDate &date);
    void onPickTagsButton();

private:
    Ui::MainWindow *ui;

    QVector<Task> tasks;
    QMap<QString, bool> tags;
    enum State { default_view, new_task } state = State::default_view;
    QDate pickedDate;
    QColor taskColor;

    void updateDefaultView();
    void clearInputWindow();
    void changeState(State state);
    void addTag(const QString &tag);

};

#endif // MAINWINDOW_H

























