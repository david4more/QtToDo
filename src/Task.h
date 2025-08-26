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
    QStringList tags;
    QString type;
    QDateTime time;
    QString recurrence;
    QString color;
    QString description;
    bool completion;

    Task (QString name, QStringList tags, QString type, QDateTime time, QString recurrence, QString color, QString description, bool completion)
        : name(name), tags(tags), type(type), time(time), recurrence(recurrence), color(color), description(description), completion(completion) {};

    Task(const QJsonObject &obj)
        : Task(
            obj["name"].toString(),
            [&] { QStringList tags;
                for (const auto &tag : obj["tags"].toArray())
                    tags.append(tag.toString());
                return tags;
            }(),
            obj["type"].toString(),
            QDateTime::fromString(obj["time"].toString(), Qt::ISODate),
            obj["recurrence"].toString(),
            obj["color"].toString(),
            obj["description"].toString(),
            obj["completion"].toBool()
            ) {};


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

    void addTag(const QString &tag) { if (!tags.contains(tag)) tags.append(tag); }

    void removeTag(const QString &tag) { tags.removeAll(tag); }

    QString tagsString() const {
        return tags.join(", ");
    }

    // NOT YET USED
    /*

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
