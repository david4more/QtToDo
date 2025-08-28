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

    static QDate pickedDate;

private slots:
    void onBoxChecked(bool checked);

private:
    void setupTask();
    QString getStyle(bool checked);
    Ui::TaskWidget *ui;
    Task *task;
};

#endif // TASKWIDGET_H
