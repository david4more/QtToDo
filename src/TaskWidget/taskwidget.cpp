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
    ui->tagsLabel->setText(task.tags);
    ui->timeLabel->setText(task.time.time().toString("hh:mm"));
    ui->completionBox->setCheckState(task.completion ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::taskStyle.arg(task.color));
}
