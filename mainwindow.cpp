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
    ui->todayLabel->setText(tasks[0].name);
}

void MainWindow::on_newTaskButton_clicked()
{
    if (state == State::default_view) {
        ui->newTaskButton->setText("Cancel");

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
    if (!validateInputs())
        return;

    QJsonArray tasksArray;
    QFile file(defaultFileName);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        if (!doc.isArray())
            return;

        tasksArray = doc.array();
        file.close();
    }

    Task task(ui->taskNameEdit->text(),
              ui->taskTimeEdit->dateTime(),
              Qt::blue,
              QDateTime(),
              "");
    QJsonObject obj;
    task.writeToJson(obj);
    tasksArray.append(obj);

    if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        QJsonDocument outDoc(tasksArray);
        file.write(outDoc.toJson());
        file.close();
    }

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

bool MainWindow::validateInputs()
{
    bool valid = true;

    auto validation = [this, &valid](QWidget *widget, std::function<bool()> check)
    {
        if (check()) {
            widget->setStyleSheet(errorStyle);
            valid = false;
        } else
            clearStyle(widget);
    };

    validation(ui->taskNameEdit, [this](){ return ui->taskNameEdit->text().trimmed().isEmpty(); });
    validation(ui->taskTimeEdit, [this](){ return ui->taskTimeEdit->time() == QTime(0,0); });

    return valid;
}























