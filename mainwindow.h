#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QTime>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QMessageBox>

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

    void on_newTaskButton_clicked();

    void on_addTaskButton_clicked();

    void on_taskDescriptionButton_clicked();

private:
    Ui::MainWindow *ui;

    void updateDefaultView(const QDate &date);

    enum State { default_view, new_task } state = State::default_view;
    void changeState(State state);

    const QString errorStyle = "background-color: #5a1f1f;";
    void clearStyle(QWidget *widget);

    void clearInputWindow();
    bool validateInputs();

    QString defaultFileName = "tasks.json";
    struct Task
    {
        Task(QString name, QDateTime time, QColor color, QDateTime deadline, QString description)
            : name(name), time(time), color(color), deadline(deadline), description(description) { };
        Task(){};

        void writeToJson(QJsonObject &obj) const
        {
            obj["name"] = name;
            obj["time"] = time.toString(Qt::ISODate);
            obj["color"] = color.name();
            obj["deadline"] = deadline.toString(Qt::ISODate);
            obj["description"] = description;
        }

        static Task fromJson(const QJsonObject &obj) {
            Task t;
            t.name = obj["name"].toString();
            t.time = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
            t.color = QColor(obj["color"].toString());
            t.deadline = QDateTime::fromString(obj["deadline"].toString(), Qt::ISODate);
            t.description = obj["description"].toString();
            return t;
        }

        QString name;
        QDateTime time;
        QColor color;
        QDateTime deadline;
        QString description;
    };
    QVector<Task> tasks;
};
#endif // MAINWINDOW_H
