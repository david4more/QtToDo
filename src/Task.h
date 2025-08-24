#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>
#include <QStringList>

struct Task
{
    QString name;
    QString tags;
    QString priority;
    QString type;
    QDateTime time;
    QString recurrence;
    QColor color;
    QString description;
    bool completion;

    Task (QString name, QString tags, QString priority, QString type, QDateTime time, QString recurrence, QColor color, QString description, bool completion)
        : name(name), tags(tags), priority(priority), type(type), time(time), recurrence(recurrence), color(color), description(description), completion(completion) {};

    Task(const QJsonObject &obj) {
        name = obj["name"].toString();
        tags = obj["tags"].toString();
        priority = obj["priority"].toString();
        type = obj["type"].toString();
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
        obj["type"] = type;
        obj["time"] = time.toString(Qt::ISODate);
        obj["recurrence"] = recurrence;
        obj["color"] = color.name();
        obj["description"] = description;
        obj["completion"] = completion;

        return obj;
    }

    QStringList tagList() const {
        return tags.split(",", Qt::SkipEmptyParts);
    }

    // NOT YET USED

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

};


#endif // TASK_H
