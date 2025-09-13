#include "taskwidget.h"
#include "widgets/ui_taskwidget.h"
#include "../Task.h"
#include "../Resourses.h"
#include <QMessageBox>

QDate TaskWidget::currentDate;

TaskWidget::~TaskWidget()
{
    delete ui;
    this->update();
}

TaskWidget::TaskWidget(QWidget *parent, Task *task)
    : QWidget(parent),
    ui(new Ui::TaskWidget),
    task(task)
{
    ui->setupUi(this);
    connect(ui->completionBox, &QCheckBox::clicked, this, &TaskWidget::onBoxChecked);
    connect(ui->expandButton, &QToolButton::clicked, this, &TaskWidget::onExpandClicked);
    connect(ui->descriptionText, &QTextEdit::textChanged, this, &TaskWidget::onDescriptionChanged);
    connect(ui->descriptionSaveButton, &QToolButton::clicked, this, &TaskWidget::onSaveButton);
    connect(ui->deleteButton, &QToolButton::clicked, this, &TaskWidget::onDeleteButton);
    connect(ui->descriptionCancelButton, &QToolButton::clicked, this, &TaskWidget::onCancelButton);
    connect(ui->editButton, &QToolButton::clicked, this, [task, this]{ emit editTask(task); });

    taskCompleted = task->isCompleted(currentDate);

    ui->nameLabel->setText(task->getName());
    ui->tagsLabel->setText(task->getTags().join(", "));
    ui->completionBox->setCheckState(taskCompleted ? Qt::Checked : Qt::Unchecked);
    ui->line->setStyleSheet(Res::Style::task.arg(task->getColor()));
    ui->expandButton->setIcon(QIcon(":/icons/expand"));
    ui->deleteButton->setIcon(QIcon(":/icons/delete"));
    ui->editButton->setIcon(QIcon(":/icons/edit"));
    ui->descriptionText->setText(task->getDescription());

    QString text;
    switch (task->getRec())
    {
    case Res::Rec::CustomDays:
    {
        auto days = task->getRecDays();
        if (days.empty())
            break;

        QLocale loc;
        for (int i = 1; i <= 7; ++i) {
            if (days.contains(Qt::DayOfWeek(i)))
                text += loc.dayName(Qt::DayOfWeek(i), QLocale::LongFormat) + ", ";
        }
        text.chop(2);
    }
        break;
    case Res::Rec::CustomInterval:
        text = "Every " + QString::number(task->getRecInterval()) + " days.";
        break;
    case Res::Rec::None:
        ui->titleLabel->setText("Completion");
        break;
    default:
        text = Res::recStrings[task->getRec()];
        break;
    }
    ui->recurrenceLabel->setText(text);
    updateStyle();
    ui->expandingWidget->setVisible(false);

    switch (task->getType())
    {
    case Res::Type::DueTime:
        ui->timeLabel->setText("Begin at " + task->getDateTime().time().toString("hh:mm"));
        break;
    case Res::Type::Deadline:
        ui->timeLabel->setText("Finish by " + task->getDateTime().time().toString("hh:mm"));
        break;
    case Res::Type::Default:
        if (Res::lightMode) {
            ui->timeLabel->setText("Created at: " + task->getDateTime().time().toString("hh:mm"));
            return;
        }

        if (currentDate == task->getDateTime().date())
            ui->timeLabel->setText(task->getDateTime().time().toString("hh:mm"));
        else if (currentDate.month() == task->getDateTime().date().month() && currentDate.year() == task->getDateTime().date().year())
            ui->timeLabel->setText(task->getDateTime().date().toString("d") + Res::getSuffix(task->getDateTime().date().day()) + " at " + task->getDateTime().time().toString("hh:mm"));
        else if (currentDate.year() == task->getDateTime().date().year())
            ui->timeLabel->setText(task->getDateTime().date().toString("MMMM d") + Res::getSuffix(task->getDateTime().date().day()) + " at " + task->getDateTime().time().toString("hh:mm"));
        else
            ui->timeLabel->setText(task->getDateTime().date().toString("dd.MM.yyyy") + " at " + task->getDateTime().time().toString("hh:mm"));
        break;
    }

}

void TaskWidget::onDeleteButton()
{
    auto confirmation = QMessageBox::question(this, "Confirm", "Do you want to delete this task?", QMessageBox::Cancel | QMessageBox::Yes);
    if (confirmation == QMessageBox::Yes) {
        emit deleteTask(task);
        delete this;
    }
}

void TaskWidget::onSaveButton()
{
    task->setDescription(ui->descriptionText->toPlainText());
    ui->descriptionSaveButton->setVisible(false);
    ui->descriptionCancelButton->setVisible(false);
    ui->descriptionText->clearFocus();
}

void TaskWidget::onCancelButton()
{
    ui->descriptionText->setText(task->getDescription());
    ui->descriptionSaveButton->setVisible(false);
    ui->descriptionCancelButton->setVisible(false);
    ui->descriptionText->clearFocus();
}

void TaskWidget::onExpandClicked()
{
    // here lays QPropertyAnimation. It has never stopped jerking. Rest in peace.
    ui->descriptionText->clearFocus();
    ui->expandingWidget->setVisible(!ui->expandingWidget->isVisible());

    QPixmap pix(":/icons/expand");
    if (ui->expandingWidget->isVisible())
        pix = pix.transformed(QTransform().scale(1, -1));
    ui->expandButton->setIcon(QIcon(pix));

    if (ui->expandingWidget->isVisible())
        ui->descriptionText->setText(task->getDescription());

    parentWidget()->layout()->update();
}

void TaskWidget::onDescriptionChanged()
{
    bool visibility = ui->descriptionText->toPlainText() != task->getDescription();
    ui->descriptionSaveButton->setVisible(visibility);
    ui->descriptionCancelButton->setVisible(visibility);

    QTextDocument* doc = ui->descriptionText->document();
    ui->descriptionText->setFixedHeight(doc->size().height() + 5);
}

void TaskWidget::onBoxChecked(bool checked)
{
    task->checked(checked, QDateTime(currentDate, QTime()));
    taskCompleted = checked;
    updateStyle();
}

void TaskWidget::updateStyle()
{
    ui->nameLabel->setStyleSheet(getStyle(taskCompleted));
    ui->tagsLabel->setStyleSheet(getStyle(taskCompleted));
    ui->timeLabel->setStyleSheet(getStyle(taskCompleted));

    if (task->getRec() == Res::Rec::None){
        if (taskCompleted)
            ui->recurrenceLabel->setText(Res::getFancyDate(task->getSingleCompletion().date()) + " at " + task->getSingleCompletion().time().toString("hh':'mm"));
        else
            ui->recurrenceLabel->setText("Not completed");
    }
}

QString TaskWidget::getStyle(bool checked)
{
    QString style = Res::Style::color.arg(Res::Color::white);
    return checked ? (style + "text-decoration: line-through;") : style;
}

void TaskWidget::setDate(QDate date)
{
    currentDate = date;
}
