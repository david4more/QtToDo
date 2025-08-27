#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../resourses.h"

QDate TaskWidget::pickedDate;

TaskWidget::~TaskWidget()
{
    delete ui;
}

TaskWidget::TaskWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TaskWidget)
{
    ui->setupUi(this);

    ui->nameLabel->setStyleSheet(Res::colorStyle.arg(Res::white));
    ui->tagsLabel->setStyleSheet(Res::colorStyle.arg(Res::white));
    ui->timeLabel->setStyleSheet(Res::colorStyle.arg(Res::white));
}

void TaskWidget::setTask(const Task &task)
{
    ui->nameLabel->setText(task.name);
    ui->tagsLabel->setText(task.tags.join(", "));
    ui->completionBox->setCheckState(task.completion ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::taskStyle.arg(task.color));

    if (task.type == Res::dueType)
        ui->timeLabel->setText("Begin at " + task.time.time().toString("hh:mm"));
    else if (task.type == Res::deadlineType)
        ui->timeLabel->setText("Finish by " + task.time.time().toString("hh:mm"));
    else if (task.type == Res::defaultType)
    {
        if (Res::lightMode) {
            ui->timeLabel->setText("Created at: " + task.time.time().toString("hh:mm"));
            return;
        }

        if (pickedDate == task.time.date())
            ui->timeLabel->setText(task.time.time().toString("hh:mm"));
        else if (pickedDate.month() == task.time.date().month() && pickedDate.year() == task.time.date().year())
            ui->timeLabel->setText(task.time.date().toString("d") + Res::getSuffix(task.time.date().day()) + " at " + task.time.time().toString("hh:mm"));
        else if (pickedDate.year() == task.time.date().year())
            ui->timeLabel->setText(task.time.date().toString("MMMM d") + Res::getSuffix(task.time.date().day()) + " at " + task.time.time().toString("hh:mm"));
        else
            ui->timeLabel->setText(task.time.date().toString("dd.MM.yyyy") + " at " + task.time.time().toString("hh:mm"));
    }
}
