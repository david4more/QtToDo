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

    void onNewTaskButton();

    void onAddTaskButton();


private:
    Ui::MainWindow *ui;

    struct Task
    {
        QString name;
        QString tags;
        QString priority;
        QDateTime time;
        QString recurrence;
        QColor color;
        QString description;
        bool completion;

        Task (QString name, QString tags, QString priority, QDateTime time, QString recurrence, QColor color, QString description, bool completion)
            : name(name), tags(tags), priority(priority), time(time), recurrence(recurrence), color(color), description(description), completion(completion) {};

        Task(const QJsonObject &obj) {
            name = obj["name"].toString();
            tags = obj["tags"].toString();
            priority = obj["priority"].toString();
            time = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
            recurrence = obj["recurrence"].toString();
            color = QColor(obj["color"].toString());
            description = obj["description"].toString();
            completion = obj["completion"].toBool();
        }

        QJsonObject toJson() const {
            QJsonObject obj;
            obj["name"] = name;
            obj["tags"] = tags;
            obj["priority"] = priority;
            obj["time"] = time.toString(Qt::ISODate);
            obj["recurrence"] = recurrence;
            obj["color"] = color.name();
            obj["description"] = description;
            obj["completion"] = completion;

            return obj;
        }

        void toJson(QJsonObject &obj) {
            obj["name"] = name;
            obj["tags"] = tags;
            obj["priority"] = priority;
            obj["time"] = time.toString(Qt::ISODate);
            obj["recurrence"] = recurrence;
            obj["color"] = color.name();
            obj["description"] = description;
            obj["completion"] = completion;
        }

        QStringList tagList() const {
            return tags.split(",", Qt::SkipEmptyParts);
        }
    };
    QVector<Task> tasks;
    const QString errorStyle = "background-color: #5a1f1f;";
    enum State { default_view, new_task } state = State::default_view;
    QString defaultFileName = "tasks.json";
    QDate pickedDate;

    void updateDefaultView(const QDate &date);
    void changeState(State state);
    void clearInputWindow();
    bool validateInputs();

};


























#endif // MAINWINDOW_H
