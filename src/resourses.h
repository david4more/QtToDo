#ifndef RESOURSES_H
#define RESOURSES_H

#include <QString>

namespace Res
{
    inline const QString defaultColor = "#7ebffc";          // green: d0ea27 6fe2b8 5ae84f 4fce46 blue: 4fa9ff 6fb9ff 00ffd5 7ebffc other: f9a245 d66fff ff66ff
    inline const QString green = "#3e8e41";
    inline const QString yellow = "#d9a600";
    inline const QString red = "#c0392b";
    inline const QString white = "#eeeeee";
    inline const QString error = "#5a1f1f";
    inline const QString colorStyle = "color: %1;";
    inline const QString backgroundStyle = "background-color: %1;";
    inline const QString taskStyle = "border-radius: 2px; background-color: %1;";
    inline const QString tasksFileName = "tasks.json";
    inline const QString prefsFileName = "preferences.json";
    inline const QString defaultType = "Default", dueType = "Due time", deadlineType = "Deadline";
    inline constexpr bool lightMode = false;

    inline const QString getSuffix(int day)
    {
        QString suffix;
        if (day % 10 == 1 && day != 11)       suffix = "st";
        else if (day % 10 == 2 && day != 12)  suffix = "nd";
        else if (day % 10 == 3 && day != 13)  suffix = "rd";
        else                                  suffix = "th";
        return suffix;
    }
};

#endif // RESOURSES_H
