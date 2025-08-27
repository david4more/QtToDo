#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>

struct Task
{
    QString name;
    QList<QString> tags;
    QString type;
    QDateTime time;
    QString recurrence;
    QString color;
    QString description;
    bool completion;

    Task (QString name, QList<QString> tags, QString type, QDateTime time, QString recurrence, QString color, QString description, bool completion)
        : name(name), tags(tags), type(type), time(time), recurrence(recurrence), color(color), description(description), completion(completion) {};

    Task(const QJsonObject &obj) :
        name(obj["name"].toString()),
        tags(jsonArrayToStringList(obj["tags"].toArray())),
        type(obj["type"].toString()),
        time(QDateTime::fromString(obj["time"].toString(), Qt::ISODate)),
        recurrence(obj["recurrence"].toString()),
        color(obj["color"].toString()),
        description(obj["description"].toString()),
        completion(obj["completion"].toBool())
    {}

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["tags"] = QJsonArray::fromStringList(tags);
        obj["type"] = type;
        obj["time"] = time.toString(Qt::ISODate);
        obj["recurrence"] = recurrence;
        obj["color"] = color;
        obj["description"] = description;
        obj["completion"] = completion;

        return obj;
    }

private:
    QList<QString> jsonArrayToStringList(const QJsonArray &array) {
        QList<QString> list;
        list.reserve(array.size());
        for (const auto &value : array)
            list.append(value.toString());
        return list;
    }

    // NOT YET USED
    /*

    void addTag(const QString &tag) { if (!tags.contains(tag)) tags.append(tag); }

    void removeTag(const QString &tag) { tags.removeAll(tag); }

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
    */
};


#endif // TASK_H
