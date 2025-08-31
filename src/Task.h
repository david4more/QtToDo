#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>
#include <QMessageBox>

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

    const bool isRecursive() const {
        if (recurrence == "")
            QMessageBox::critical(nullptr, "Error", "Recurrence field is empty);");
        return (recurrence != "0");
    };

    const bool isInterval() const {
        bool ok = false;
        recurrence.toInt(&ok);
        return ok;
    };

    const bool completed(const QDate &currentDate) const {
        if (!this->isRecursive()) {
            return !this->completion.isEmpty();
        } else {
            QStringList completedDates = this->completion.split(' ', Qt::SkipEmptyParts);
            return (completedDates.contains(currentDate.toString(Qt::ISODate)));
        }
    };

    const void checked(const bool& checked, const QDate &currentDate)
    {
        QString date = currentDate.toString(Qt::ISODate);
        if (!this->isRecursive()) {
            this->completion = checked ? date : "";
        } else {
            QStringList completedDates = this->completion.split(' ', Qt::SkipEmptyParts);

            if (checked)
                completedDates.append(date);
            else
                completedDates.removeAll(date);

            this->completion = completedDates.join(' ');
        }
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
