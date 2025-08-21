#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "taskwidget.h"

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

    ui->todayGroup->setTitle(QDate::currentDate().toString("dddd, d MMMM") + ": Today's tasks");

    QFile file(defaultFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(Task(val.toObject()));
    }
    pickedDate = QDate::currentDate();

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
        ui->taskNameEdit->setStyleSheet(errorStyle);
        return;
    }

    QFile file(defaultFileName);
    if (!file.open(QIODevice::ReadWrite)) {
        QMessageBox::critical(this, "File Error", "Failed to open tasks file for read/write.");
        return;
    }

    QJsonArray tasksArray;
    QByteArray data = file.readAll();
    QJsonDocument doc;
    if (!data.isEmpty()) {
        doc = QJsonDocument::fromJson(data);
        if (doc.isArray())
            tasksArray = doc.array();
        else {
            file.close();
            QMessageBox::critical(this, "File Error", "Tasks file is corrupted.");
            return;
        }
    }

    Task task(
        ui->taskNameEdit->text(),
        "",
        "",
        QDateTime(ui->taskDateEdit->date(), ui->taskTimeEdit->time()),
        "",
        Qt::blue,
        ui->taskDescriptionEdit->toPlainText(),
        false);

    tasks.append(task);
    tasksArray.append(task.toJson());

    file.resize(0);
    doc = QJsonDocument(tasksArray);
    file.write(doc.toJson());
    file.close();


    changeState(State::default_view);
}

void MainWindow::updateDefaultView()
{
    QVBoxLayout *layout = qobject_cast<QVBoxLayout*>(ui->scrollAreaWidget->layout());

    QLayoutItem *child;
    while ((child = layout->takeAt(0)) != nullptr) {
        if (QWidget *w = child->widget()) {
            w->deleteLater();
        }
        delete child;
    }

    for (const Task &task : tasks)
    {
        if (task.time.date() != pickedDate)
            continue;

        TaskWidget *widget = new TaskWidget(ui->scrollAreaWidget);
        widget->setTask(task);
        layout->addWidget(widget);
    }
}

void MainWindow::clearInputWindow()
{
    ui->taskNameEdit->setStyleSheet("");
    ui->taskNameEdit->clear();
    ui->taskDescriptionEdit->clear();

    ui->taskDateEdit->setDate(QDate::currentDate());
    ui->taskTimeEdit->setTime(QTime::currentTime());
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






















