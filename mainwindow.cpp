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
    ui->stackedWidget->setCurrentWidget(ui->default_view);
}

void MainWindow::on_newTaskButton_clicked()
{
    ui->stackedWidget->setCurrentWidget(ui->new_task);

    ui->descriptionEdit->hide();
    ui->descriptionButton->show();
}


void MainWindow::on_addButton_clicked() // add task button
{
    ui->stackedWidget->setCurrentWidget(ui->default_view);
}

void MainWindow::on_descriptionButton_clicked()
{
    ui->descriptionEdit->show();
    ui->descriptionButton->hide();
}
