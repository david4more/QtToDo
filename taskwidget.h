#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include "Task.h"

namespace Ui {
class TaskWidget;
}

class TaskWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TaskWidget(QWidget *parent = nullptr);
    ~TaskWidget();

    void setTask(const Task &task);

private:
    Ui::TaskWidget *ui;
    QColor color;
};

#endif // TASKWIDGET_H
