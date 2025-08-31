#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../resourses.h"
#include <QMessageBox>

QDate TaskWidget::currentDate;

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
    connect(ui->completionBox, &QCheckBox::clicked, this, &TaskWidget::onBoxChecked);

    ui->nameLabel->setText(task->name);
    ui->tagsLabel->setText(task->tags.join(", "));
    ui->completionBox->setCheckState(taskCompleted() ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::taskStyle.arg(task->color));
    onBoxChecked(taskCompleted());

    if (task->type == Res::dueType)
        ui->timeLabel->setText("Begin at " + task->time.time().toString("hh:mm"));
    else if (task->type == Res::deadlineType)
        ui->timeLabel->setText("Finish by " + task->time.time().toString("hh:mm"));
    else if (task->type == Res::defaultType)
    {
        if (Res::mode == Res::Mode::light) {
            ui->timeLabel->setText("Created at: " + task->time.time().toString("hh:mm"));
            return;
        }

        if (currentDate == task->time.date())
            ui->timeLabel->setText(task->time.time().toString("hh:mm"));
        else if (currentDate.month() == task->time.date().month() && currentDate.year() == task->time.date().year())
            ui->timeLabel->setText(task->time.date().toString("d") + Res::getSuffix(task->time.date().day()) + " at " + task->time.time().toString("hh:mm"));
        else if (currentDate.year() == task->time.date().year())
            ui->timeLabel->setText(task->time.date().toString("MMMM d") + Res::getSuffix(task->time.date().day()) + " at " + task->time.time().toString("hh:mm"));
        else
            ui->timeLabel->setText(task->time.date().toString("dd.MM.yyyy") + " at " + task->time.time().toString("hh:mm"));
    }
}

bool TaskWidget::taskCompleted()
{
    return (task->completion != "");
}

void TaskWidget::setDate(QDate date)
{
    currentDate = date;
}

void TaskWidget::onBoxChecked(bool checked)
{
    task->completion = (checked) ? QDateTime::currentDateTime().toString(Qt::ISODate) : "";
    ui->nameLabel->setStyleSheet(getStyle(taskCompleted()));
    ui->tagsLabel->setStyleSheet(getStyle(taskCompleted()));
    ui->timeLabel->setStyleSheet(getStyle(taskCompleted()));
}

QString TaskWidget::getStyle(bool checked)
{
    QString style = Res::colorStyle.arg(Res::white);
    return checked ? (style + "text-decoration: line-through;") : style;
}
