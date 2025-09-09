#ifndef TASKWIDGET_H
#define TASKWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QVBoxLayout>
#include <QTimer>

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

signals:
    void deleteTask(Task* task);

private slots:
    void onBoxChecked(bool checked);
    void onExpandClicked();
    void onDescriptionChanged();
    void onSaveButton();
    void onCancelButton();
    void onDeleteButton();

private:
    QString getStyle(bool checked);
    void updateStyle();
    bool taskCompleted;
    Ui::TaskWidget *ui;
    Task *task;
    static QDate currentDate;
};

#endif // TASKWIDGET_H
