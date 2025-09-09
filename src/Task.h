#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QColor>
#include <QJsonObject>
#include <QStringList>
#include <QJsonArray>
#include <QMessageBox>
#include "Resourses.h"

class Task
{
public:

    Task(QString name, QVector<QString> tags, Res::Type type, QDateTime time, QColor color, QString description, Res::Rec recurrence)
        : name(std::move(name)),
        tags(std::move(tags)),
        type(std::move(type)),
        time(std::move(time)),
        color(std::move(color)),
        description(std::move(description)),
        recurrence(std::move(recurrence))
    {
        if (recurrence != Res::Rec::CustomDays)
            recInterval = static_cast<int>(recurrence);
    }

    void setRecInterval(const int& x) { recInterval = x; }
    void setRecDays(const QSet<Qt::DayOfWeek>& x) { recDays = x; }

    Task(const QJsonObject &obj) :
        name(obj["name"].toString()),
        type(Res::typeStrings.key(obj["type"].toString())),
        time(QDateTime::fromString(obj["time"].toString(), Qt::ISODate)),
        color(obj["color"].toString()),
        description(obj["description"].toString())
    {
        QJsonArray tagsArray = obj["tags"].toArray();
        for (const auto &val : tagsArray)
            tags.append(val.toString());

        recurrence = Res::recStrings.key(obj["recurrence"].toString());
        if (recurrence == Res::Rec::CustomInterval)
            recInterval = obj["rec interval"].toInt();
        else if (recurrence == Res::Rec::CustomDays) {
            QJsonArray days = obj["rec days"].toArray();
            for (const auto &val : days)
                recDays.insert(static_cast<Qt::DayOfWeek>(val.toInt()));
        } else
            recInterval = static_cast<int>(recurrence);

        if (recurrence == Res::Rec::None)
            singleCompletion = QDateTime::fromString(obj["completion"].toString(), Qt::ISODate);
        else {
            QJsonArray completionArray = obj["completion"].toArray();
            for (const auto &val : completionArray)
                completion.insert(QDate::fromString(val.toString(), Qt::ISODate));
        }
    }

    QJsonObject toJson() const {
        QJsonObject obj;
        obj["name"] = name;
        obj["tags"] = QJsonArray::fromStringList(tags);
        obj["type"] = Res::typeStrings.value(type);
        obj["time"] = time.toString(Qt::ISODate);
        obj["color"] = color.name();
        obj["description"] = description;

        obj["recurrence"] = Res::recStrings[recurrence];
        if (recurrence == Res::Rec::CustomInterval)
            obj["rec interval"] = recInterval;
        else if (recurrence == Res::Rec::CustomDays) {
            QJsonArray daysArray;
            for (const auto& val : recDays)
                daysArray.append(val);
            obj["rec days"] = daysArray;
        }

        if (recurrence == Res::Rec::None)
            obj["completion"] = (singleCompletion.isValid()) ? singleCompletion.toString(Qt::ISODate) : "";
        else {
            QJsonArray completionArray;
            for (const auto &d : completion)
                completionArray.append(d.toString(Qt::ISODate));
            obj["completion"] = completionArray;
        }

        return obj;
    }

    void checked(const bool& checked, const QDateTime &currentDate)
    {
        if (recurrence == Res::Rec::None) {
            if (checked)
                singleCompletion = QDateTime::currentDateTime();
            else
                singleCompletion = QDateTime();
        }
        else {
            if (checked)
                completion.insert(currentDate.date());
            else
                completion.remove(currentDate.date());
        }
    }

    void addTag(const QString &tag) { if (!tags.contains(tag)) tags.append(tag); }

    void removeTag(const QString &tag) { tags.removeAll(tag); }

    void setDescription(const QString &text) { description = text; }

    // info/getters
    bool isCompleted(const QDate &currentDate) const {
        if (recurrence == Res::Rec::None)
            return singleCompletion.isValid();
        else
            return completion.contains(currentDate);
    };
    bool isRecursive() const { return (recurrence != Res::Rec::None); }

    QDateTime getDateTime() const { return time; }
    QVector<QString> getTags() const { return tags; }
    Res::Type getType() const { return type; }
    Res::Rec getRec() const { return recurrence; }
    QString getColor() const { return color.name(); }
    int getRecInterval() const { return recInterval; }
    QSet<Qt::DayOfWeek> getRecDays() const { return recDays; }
    QDateTime getSingleCompletion() const { return singleCompletion; }
    QString getName() const { return name; }
    QString getDescription() const { return description; }

private:
    Res::Rec recurrence = Res::Rec::None;
    QSet<Qt::DayOfWeek> recDays{};
    int recInterval = 0;
    QSet<QDate> completion{};
    QDateTime singleCompletion = QDateTime();

    QString name;
    Res::Type type;
    QColor color;
    QVector<QString> tags;
    QDateTime time;

    QString description = "";
};


#endif // TASK_H
