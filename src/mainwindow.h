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
#include <QScopedPointer>
#include <QRandomGenerator>

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
    void onPickColorButton();
    void onDateClick(const QDate &date);
    void onPickTagsButton();
    void onRecurrenceBox(const QString& text);

private:
    Ui::MainWindow *ui;

    QVector<Task> tasks;
    QMap<QString, bool> tags;
    enum State { default_view, new_task } state = State::default_view;

    QDate pickedDate;
    struct {
        QColor color;
        QString recurrence;
        void clear() { color = QColor(); recurrence = ""; }
    } data;

    void setupUI();
    void saveTasks();
    void savePreferences();
    void loadFiles();

    void updateDefaultView();
    void clearInputWindow();
    void changeState(State state);
    void addTag(const QString &tag);

};

#endif // MAINWINDOW_H

























