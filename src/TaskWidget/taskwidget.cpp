#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../resourses.h"
#include <QMessageBox>

QDate TaskWidget::pickedDate;

TaskWidget::~TaskWidget()
{
    delete ui;
}

TaskWidget::TaskWidget(QWidget *parent, Task *task)
    : QWidget(parent),
    ui(new Ui::TaskWidget),
    task(task)
{
    ui->setupUi(this);
    setupTask();
    connect(ui->completionBox, &QCheckBox::clicked, this, &TaskWidget::onBoxChecked);
}

void TaskWidget::onBoxChecked(bool checked)
{
    task->completion = checked;
    ui->nameLabel->setStyleSheet(getStyle(task->completion));
    ui->tagsLabel->setStyleSheet(getStyle(task->completion));
    ui->timeLabel->setStyleSheet(getStyle(task->completion));
}

QString TaskWidget::getStyle(bool checked)
{
    QString style = Res::colorStyle.arg(Res::white);
    return checked ? (style + "text-decoration: line-through;") : style;
}

void TaskWidget::setupTask()
{
    ui->nameLabel->setText(task->name);
    ui->tagsLabel->setText(task->tags.join(", "));
    ui->completionBox->setCheckState(task->completion ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::taskStyle.arg(task->color));
    onBoxChecked(task->completion);

    if (task->type == Res::dueType)
        ui->timeLabel->setText("Begin at " + task->time.time().toString("hh:mm"));
    else if (task->type == Res::deadlineType)
        ui->timeLabel->setText("Finish by " + task->time.time().toString("hh:mm"));
    else if (task->type == Res::defaultType)
    {
        if (Res::lightMode) {
            ui->timeLabel->setText("Created at: " + task->time.time().toString("hh:mm"));
            return;
        }

        if (pickedDate == task->time.date())
            ui->timeLabel->setText(task->time.time().toString("hh:mm"));
        else if (pickedDate.month() == task->time.date().month() && pickedDate.year() == task->time.date().year())
            ui->timeLabel->setText(task->time.date().toString("d") + Res::getSuffix(task->time.date().day()) + " at " + task->time.time().toString("hh:mm"));
        else if (pickedDate.year() == task->time.date().year())
            ui->timeLabel->setText(task->time.date().toString("MMMM d") + Res::getSuffix(task->time.date().day()) + " at " + task->time.time().toString("hh:mm"));
        else
            ui->timeLabel->setText(task->time.date().toString("dd.MM.yyyy") + " at " + task->time.time().toString("hh:mm"));
    }
}
