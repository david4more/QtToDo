#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
class Task;

namespace Ui {
class TaskWidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(QWidget *parent, Task *task);
    ~TaskWidget();

    static void setDate(QDate date);

private slots:
    void onBoxChecked(bool checked);

private:
    QString getStyle(bool checked);
    void updateStyle();
    bool taskCompleted;
    Ui::TaskWidget *ui;
    Task *task;
    static QDate currentDate;
};

#endif // TASKWIDGET_H
