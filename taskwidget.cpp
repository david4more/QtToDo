#include "taskwidget.h"
#include "ui_taskwidget.h"

TaskWidget::~TaskWidget()
{
    delete ui;
}

TaskWidget::TaskWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::TaskWidget)
{
    ui->setupUi(this);
}

void TaskWidget::setTask(const Task &task)
{
    ui->nameLabel->setText(task.name);
    ui->tagsLabel->setText(task.tags);
    ui->timeLabel->setText(task.time.time().toString());
    ui->completionBox->setCheckState(task.completion ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(QString("background-color: %1").arg((task.priority == "Low") ? "green" : (task.priority == "High") ? "red" : "yellow"));
    color = task.color;

    // fill with color here
}
