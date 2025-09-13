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
#include <QTimer>
#include <QPointer>
#include <QFileInfo>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QFontDialog>

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
    void onSettingsColorButton();
    void onStartTimerButton();
    void onTimerTimeout();
    void onTimerTimeButton();
    void onPomoFontButton();
    void onEditTask(Task* task);
    void onResetSettings();

private:
    Ui::MainWindow *ui;

    QVector<Task*> tasks;
    QMap<QString, bool> tags;
    enum State { default_view, new_task, settings, pomodoro } state = State::default_view;
    QDate pickedDate;
    bool showCompletedTasks;
    struct {
        QString getTime() { return QString("%1:%2").arg(static_cast<int>(currentTime / 60), 2, 10, QChar('0')).arg(static_cast<int>(currentTime % 60), 2, 10, QChar('0')); };
        QString resetTime(bool longBreakAccepted) {
            isWork = !isWork;

            if (!longBreakAccepted)
                currentTime = 60 * (isWork ? work : rest);
            else {
                currentTime = 60 * rest * cycle;
                cycle = 0;
            }
            if (isWork) ++cycle;

            return ((isWork ? "Work" : (longBreakAccepted ? "Long break" : "Break")) + QString(": cycle ") + QString::number(cycle));
        };
        void init() { currentTime = 60 * work; isWork = true; timerRunning = false; cycle = 1; }
        bool timerRunning;
        bool isWork;
        int work;
        int rest;
        int currentTime;
        int cycle;
        int bigRestCycle;
        double alarmVolume;
        QMediaPlayer audioPlayer;
        QAudioOutput audioOutput;
        QString fontFamily;
        QTimer timer;
    } pomo;
    QVector<QColor> customColors;

    void setupUI();
    void saveTasks();
    void savePreferences();
    void loadFiles();

    QColor getColor(QString title, QColor currentColor);
    bool isPresent(const Task* task, const QDate& date);
    void updateLeftPanel();
    void updateCalendar();
    void resetTimer();
    void updateDefaultView();
    void clearInputWindow();
    void changeState(State state);
    void errorStyleTimer(QWidget* widget);
};

#endif // MAINWINDOW_H

























