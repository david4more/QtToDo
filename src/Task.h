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
    QString type;
    QDateTime time;
    QString recurrence;
    QString color;
    QString description;
    bool completion;

    Task (QString name, QString tags, QString type, QDateTime time, QString recurrence, QString color, QString description, bool completion)
        : name(name), tags(tags), type(type), time(time), recurrence(recurrence), color(color), description(description), completion(completion) {};

    Task(const QJsonObject &obj) {
        name = obj["name"].toString();
        tags = obj["tags"].toString();
        type = obj["type"].toString();
        time = QDateTime::fromString(obj["time"].toString(), Qt::ISODate);
        recurrence = obj["recurrence"].toString();
        color = obj["color"].toString();
        description = obj["description"].toString();
        completion = obj["completion"].toBool();
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["tags"] = tags;
        obj["type"] = type;
        obj["time"] = time.toString(Qt::ISODate);
        obj["recurrence"] = recurrence;
        obj["color"] = color;
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
        obj["type"] = type;
        obj["time"] = time.toString(Qt::ISODate);
        obj["recurrence"] = recurrence;
        obj["color"] = color;
        obj["description"] = description;
        obj["completion"] = completion;
    }

};


#endif // TASK_H
