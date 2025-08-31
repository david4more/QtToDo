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
    QString color;
    QString description;
    QString completion;

    QString recurrence;

    Task (QString name, QList<QString> tags, QString type, QDateTime time, QString color, QString description, QString completion, QString recurrence)
        : name(name), tags(tags), type(type), time(time), color(color), description(description), completion(completion), recurrence(recurrence) {};

    Task(const QJsonObject &obj) :
        name(obj["name"].toString()),
        tags(jsonArrayToStringList(obj["tags"].toArray())),
        type(obj["type"].toString()),
        time(QDateTime::fromString(obj["time"].toString(), Qt::ISODate)),
        color(obj["color"].toString()),
        description(obj["description"].toString()),
        completion(obj["completion"].toString()),

        recurrence(obj["recurrence"].toString())
    {}

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["tags"] = QJsonArray::fromStringList(tags);
        obj["type"] = type;
        obj["time"] = time.toString(Qt::ISODate);
        obj["color"] = color;
        obj["description"] = description;
        obj["completion"] = completion;

        obj["recurrence"] = recurrence;

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
    */
};


#endif // TASK_H
