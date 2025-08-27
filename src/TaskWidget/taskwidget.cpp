#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../resourses.h"

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

    if (task.type == Res::defaultType)
        ui->timeLabel->setText("Added at: " + task.time.time().toString("hh:mm"));
    else if (task.type == Res::dueType)
        ui->timeLabel->setText("Begin at: " + task.time.time().toString("hh:mm"));
    else if (task.type == Res::deadlineType)
        ui->timeLabel->setText("Finish by: " + task.time.time().toString("hh:mm"));
}
