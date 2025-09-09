#ifndef RESOURSES_H
#define RESOURSES_H

#include <QString>
#include <QMap>
#include <QMessageBox>
#include <QDate>

namespace Res
{
    namespace Color {
        inline const QString def = "#7ebffc";
        inline const QString green = "#3e8e41";
        inline const QString yellow = "#d9a600";
        inline const QString red = "#c0392b";
        inline const QString white = "#eeeeee";
        inline const QString error = "#5a1f1f";
    }

    namespace Style {
        inline const QString color = "color: %1;";
        inline const QString background = "background-color: %1;";
        inline const QString task = "border-radius: 2px; background-color: %1;";
        inline const QString noTasks = "color: gray; font-style: italic; font-size: 14px;";
    }

    namespace Files {
        inline const QString tasks = "tasks.json";
        inline const QString prefs = "preferences.json";
    }

    enum class Type { Default = 0, DueTime = 1, Deadline = 2 };
    inline const QMap<Type, QString> typeStrings {
        {Type::Default, "Default"},
        {Type::DueTime, "Due time"},
        {Type::Deadline, "Deadline"}
    };

    enum class Rec { None = 0, Daily = 1, Weekly = 7, Monthly = 30, Yearly = 365, CustomInterval = -1, CustomDays = -2 };
    inline const QMap<Rec, QString> recStrings {
        { Rec::None, "None" },
        { Rec::Daily, "Daily" },
        { Rec::Weekly, "Weekly" },
        { Rec::Monthly, "Monthly" },
        { Rec::Yearly, "Yearly" },
        { Rec::CustomInterval, "Custom interval" },
        { Rec::CustomDays, "Custom days" }
    };

    inline constexpr const char* getSuffix(int day) {
        if ((day % 10 == 1) && (day != 11)) return "st";
        if ((day % 10 == 2) && (day != 12)) return "nd";
        if ((day % 10 == 3) && (day != 13)) return "rd";
        return "th";
    }
    inline QString getFancyDate(const QDate& date) {
        int day = date.day();
        QString suffix = getSuffix(day);
        QString title = date.toString("dddd, MMMM d yyyy");
        title.replace(QString::number(day), QString::number(day) + suffix);

        return title;
    }

    inline const QStringList noTasksPlaceholders = {
        "No quests today;\ntake heed and chill thy spirit,\nO mortal of fleeting time.",
        "No quests call today;\nRest, let thy spirit be chill—\nTime drifts, swift and brief.",
        "The scroll lies unread;\nchill winds whisper through the trees—\nTrials wait for dawn’s call.",
        "O wanderer, chill;\nnot all days are meant for strife,\nsome just breathe in peace.",
        "Your blade may stay still;\nlet the hours drift and chill—\nFate yet bides its time.",
        "Questless stars above;\nchill night guards thy fleeting soul—\nTomorrow may burn."
    };

    enum class Mode { Default, Light }; inline constexpr Mode mode = Mode::Default;
};

#endif // RESOURSES_H
