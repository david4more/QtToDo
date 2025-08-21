#include "mainwindow.h"
#include "./ui_mainwindow.h"

                                   MainWindow::~MainWindow()
{
    delete ui;
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->stackedWidget->setCurrentIndex(0);
    ui->taskDescriptionEdit->hide();
    ui->todayLabel->setText(QDate::currentDate().toString("dddd, d MMMM") + ": Today's tasks");

    QFile file(defaultFileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        if (!doc.isArray())
            return;

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(Task::fromJson(val.toObject()));
    }
    updateDefaultView(QDate::currentDate());
}

void MainWindow::updateDefaultView(const QDate &date)
{
    for (const Task &task : tasks)
    {
        if (task.time.date() != date)
            continue;

        // HERE SHOULD BE THE CODE
    }

    ui->taskNameLabel->setText(tasks.back().name);
    ui->taskTimeLabel->setText(tasks.back().time.toString());
}

void MainWindow::on_newTaskButton_clicked()
{
    if (state == State::default_view) {
        ui->newTaskButton->setText("Cancel");
        ui->taskTimeEdit->setDateTime(QDateTime::currentDateTime());
        changeState(State::new_task);
    }
    else if (state == State::new_task) {
        ui->newTaskButton->setText("New Task");

        clearInputWindow();
        changeState(State::default_view);
    }
}

void MainWindow::on_addTaskButton_clicked()
{
    if (ui->taskNameEdit->text().trimmed().isEmpty())
    {
        ui->taskNameEdit->setStyleSheet(errorStyle);
        return;
    }
    clearStyle(ui->taskNameEdit);


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

    Task task(ui->taskNameEdit->text(),
              ui->taskTimeEdit->dateTime(),
              Qt::blue,
              QDateTime(),
              (ui->taskDescriptionEdit->isVisible() ? ui->taskDescriptionEdit->toPlainText() : ""));
    QJsonObject obj;
    task.writeToJson(obj);
    tasksArray.append(obj);

    file.resize(0);
    doc = QJsonDocument(tasksArray);
    file.write(doc.toJson());
    file.close();

    ui->newTaskButton->setText("New Task");
    clearInputWindow();
    changeState(State::default_view);
}

void MainWindow::clearInputWindow()
{
    clearStyle(ui->taskNameEdit);
    ui->taskNameEdit->clear();

    clearStyle(ui->taskTimeEdit);
    ui->taskTimeEdit->setTime(QTime(0,0));

    ui->taskDescriptionEdit->hide();
    ui->taskDescriptionButton->show();
}

void MainWindow::on_taskDescriptionButton_clicked()
{
    ui->taskDescriptionEdit->show();
    ui->taskDescriptionButton->hide();
}

void MainWindow::clearStyle(QWidget *widget)
{
    widget->setStyleSheet("");
}

void MainWindow::changeState(State state)
{
    this->state = state;
    ui->stackedWidget->setCurrentIndex(state);
}






















