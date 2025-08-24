#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskWidget/taskwidget.h"
#include "Task.h"
#include "resourses.h"

MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect(ui->newTaskButton, &QPushButton::clicked, this, &MainWindow::onNewTaskButton);
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTaskButton);
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateClick);

    QFile file(Res::tasksFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(Task(val.toObject()));
    }

    pickedDate = QDate::currentDate();



    //                  STYLING
    // date suffix
    int day = pickedDate.day();
    QString suffix;
    if (day % 10 == 1 && day != 11)       suffix = "st";
    else if (day % 10 == 2 && day != 12)  suffix = "nd";
    else if (day % 10 == 3 && day != 13)  suffix = "rd";
    else                                  suffix = "th";
    QString title = pickedDate.toString("dddd, MMMM d");
    title.replace(QString::number(day), QString::number(day) + suffix);
    ui->todayGroup->setTitle(title + ": tasks");

    // calendar styling
    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_prevmonth")->hide();
    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_nextmonth")->hide();
    QTextCharFormat format;
    format.setForeground(QColor(Res::white));
    ui->calendarWidget->setHeaderTextFormat(format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Monday, format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Tuesday,   format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Wednesday, format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Thursday,   format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Friday, format);
    format.setForeground(QColor(Res::blue));
    ui->calendarWidget->setWeekdayTextFormat(Qt::Saturday, format);
    ui->calendarWidget->setWeekdayTextFormat(Qt::Sunday,   format);

    ui->todayGroup->setStyleSheet(Res::colorStyle.arg(Res::white));
    ui->newTaskButton->setStyleSheet(Res::colorStyle.arg(Res::white));

    ui->deadlinesListWidget->addItem("No quests today;\ntake heed and chill thy spirit,\nO mortal of fleeting time.");

    changeState(State::default_view);
}

void MainWindow::onDateClick(const QDate &date)
{
    pickedDate = date;
    updateDefaultView();
}

void MainWindow::onNewTaskButton()
{
    switch (state) {
    case State::default_view:
        changeState(State::new_task); break;
    case State::new_task:
        changeState(State::default_view); break;
    }
}

void MainWindow::onAddTaskButton()
{
    if (ui->taskNameEdit->text().trimmed().isEmpty())
    {
        ui->taskNameEdit->setStyleSheet(Res::backgroundStyle.arg(Res::error));
        return;
    }

    Task task(
        ui->taskNameEdit->text(),
        "",
        ui->taskPriorityBox->currentText(),
        ui->taskTypeBox->currentText(),
        QDateTime(ui->taskDateEdit->date(), ui->taskTimeEdit->time()),
        "",
        Qt::blue,
        ui->taskDescriptionEdit->toPlainText(),
        false);

    auto it = std::lower_bound(tasks.begin(), tasks.end(), task,
        [](const Task &a, const Task &b){ return a.time < b.time; });
    tasks.insert(it, task);

    QJsonArray tasksArray;
    for (const Task &t : tasks)
        tasksArray.append(t.toJson());

    QSaveFile file(Res::tasksFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "File Error", "Failed to open tasks file for writing.");
        return;
    }

    QJsonDocument doc(tasksArray);
    file.write(doc.toJson());
    if (!file.commit())
        QMessageBox::critical(this, "File Error", "Failed to save tasks.");

    changeState(State::default_view);
}

void MainWindow::updateDefaultView()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidget->layout());
    layout->setAlignment(Qt::AlignTop);

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QWidget *w = child->widget()) {
            w->deleteLater();
        }
        delete child;
    }

    bool hasTasks = false;
    for (const Task &task : tasks)
    {
        if (task.time.date() != pickedDate)
            continue;

        hasTasks = true;
        TaskWidget *widget = new TaskWidget();
        widget->setTask(task);
        layout->addWidget(widget);
    }

    if (!hasTasks)
    {
        QLabel *label = new QLabel("No tasks this day");
        label->setStyleSheet("color: gray; font-style: italic; font-size: 14px;");
        label->setAlignment(Qt::AlignCenter);
        layout->addWidget(label);
    }


    layout->invalidate();
    layout->activate();
}

void MainWindow::clearInputWindow()
{
    ui->taskNameEdit->setStyleSheet("");
    ui->taskNameEdit->clear();
    ui->taskPriorityBox->setCurrentIndex(0);
    ui->taskTypeBox->setCurrentIndex(0);
    ui->taskDateEdit->setDate(QDate::currentDate());
    ui->taskTimeEdit->setTime(QTime::currentTime());
    ui->taskRecurrenceBox->setCurrentIndex(0);
    ui->taskDescriptionEdit->clear();
}

void MainWindow::changeState(State state)
{
    this->state = state;

    switch (state)
    {
    case State::new_task:
        clearInputWindow();
        ui->newTaskButton->setText("Cancel");
        break;
    case State::default_view:
        updateDefaultView();
        ui->newTaskButton->setText("New Task");
        break;
    }

    ui->stackedWidget->setCurrentIndex(state);
}






















