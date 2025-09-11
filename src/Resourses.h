#ifndef RESOURSES_H
#define RESOURSES_H

#include <QString>
#include <QMap>
#include <QMessageBox>
#include <QDate>
#include <QRandomGenerator>

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

    namespace File {
        inline const QString tasks = "tasks.json";
        inline const QString prefs = "preferences.json";

        inline const QString tags = "tags";
        inline const QString defaultColor = "default color";
        inline const QString completedTasks = "show completed";
        inline const QString professionalMode = "professional mode";
        inline const QString lightMode = "light mode";
        inline const QString pomoWork = "pomo work";
        inline const QString pomoBreak = "pomo break";
        inline const QString pomoRestCycle = "pomo rest cycle";
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

    namespace Text {
        inline QString randomText(const QStringList &list) {
            return list.at(QRandomGenerator::global()->bounded(list.size()));
        }

        inline const QStringList noTasksPlaceholders = {
            "No quests today; take heed and chill thy spirit, O mortal of fleeting time.",
            "No quests call today; Rest, let thy spirit be chill — Time drifts, swift and brief.",
            "The scroll lies unread; chill winds whisper through the trees — Trials wait for dawn’s call.",
            "O wanderer, chill; not all days are meant for strife, some just breathe in peace.",
            "Your blade may stay still; let the hours drift and chill — Fate yet bides its time.",
            "Questless stars above; chill night guards thy fleeting soul — Tomorrow may burn."
        };

        inline const QStringList pomoWorkDone = {
            "Chill thy spirit",
            "Let thy soul unite with the chill",
            "Breathe, feel the chilling wind"
        };
        inline const QStringList pomoBreakDone = {
            "Let the wind guide you again",
            "Follow your soul's desires",
            "Command and conquer",
            "Unleash your will",
            "The quest thrives for your soul"
        };

        inline const QStringList pomoFirstCycle = {
            // "Discover the wonder, thrive for your soul's fuel"
            "First adventure",
            "Flight's beginning"
        };

        inline const QStringList pomoLongBreak = {
            "Willst thou experience the great chill?"
        };

        inline const QString workEnd = "Work ended";
        inline const QString breakEnd = "Break ended";
    }

    inline bool lightMode = false;
    inline bool professionalMode = false;
    inline QColor defaultColor;
};

#endif // RESOURSES_H
