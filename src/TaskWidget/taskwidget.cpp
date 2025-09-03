#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../Resourses.h"
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
    taskCompleted = task->isCompleted(currentDate);

    updateStyle();
    ui->nameLabel->setText(task->name);
    ui->tagsLabel->setText(task->getTags().join(", "));
    ui->completionBox->setCheckState(taskCompleted ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::Style::task.arg(task->getColor()));

    switch (task->getType())
    {
    case Res::Type::DueTime:
        ui->timeLabel->setText("Begin at " + task->getTime().time().toString("hh:mm"));
        break;
    case Res::Type::Deadline:
        ui->timeLabel->setText("Finish by " + task->getTime().time().toString("hh:mm"));
        break;
    case Res::Type::Default:
        if (Res::mode == Res::Mode::Light) {
            ui->timeLabel->setText("Created at: " + task->getTime().time().toString("hh:mm"));
            return;
        }

        if (currentDate == task->getTime().date())
            ui->timeLabel->setText(task->getTime().time().toString("hh:mm"));
        else if (currentDate.month() == task->getTime().date().month() && currentDate.year() == task->getTime().date().year())
            ui->timeLabel->setText(task->getTime().date().toString("d") + Res::getSuffix(task->getTime().date().day()) + " at " + task->getTime().time().toString("hh:mm"));
        else if (currentDate.year() == task->getTime().date().year())
            ui->timeLabel->setText(task->getTime().date().toString("MMMM d") + Res::getSuffix(task->getTime().date().day()) + " at " + task->getTime().time().toString("hh:mm"));
        else
            ui->timeLabel->setText(task->getTime().date().toString("dd.MM.yyyy") + " at " + task->getTime().time().toString("hh:mm"));
        break;
    }
}

void TaskWidget::setDate(QDate date)
{
    currentDate = date;
}

void TaskWidget::onBoxChecked(bool checked)
{
    task->checked(checked, currentDate);
    taskCompleted = checked;
    updateStyle();
}

void TaskWidget::updateStyle()
{
    ui->nameLabel->setStyleSheet(getStyle(taskCompleted));
    ui->tagsLabel->setStyleSheet(getStyle(taskCompleted));
    ui->timeLabel->setStyleSheet(getStyle(taskCompleted));
}

QString TaskWidget::getStyle(bool checked)
{
    QString style = Res::Style::color.arg(Res::Color::white);
    return checked ? (style + "text-decoration: line-through;") : style;
}
