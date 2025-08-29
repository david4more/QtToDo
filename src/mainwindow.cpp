#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskWidget/taskwidget.h"
#include "Task.h"
#include "resourses.h"

// Construction/desctruction, files handling
MainWindow::~MainWindow()
{
    saveTasks();
    savePreferences();
    delete ui;
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    loadFiles();
    setupUI();
    changeState(State::default_view);
}

void MainWindow::setupUI()
{
    pickedDate = QDate::currentDate();

    connect(ui->newTaskButton, &QPushButton::clicked, this, &MainWindow::onNewTaskButton);
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTaskButton);
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateClick);
    connect(ui->taskColorButton, &QPushButton::clicked, this, &MainWindow::onPickColorButton);
    connect(ui->taskTagsButton, &QPushButton::clicked, this, &MainWindow::onPickTagsButton);
    connect(ui->taskRecurrenceBox, &QComboBox::currentTextChanged, this, &MainWindow::onRecurrenceBox);

    // calendar styling
    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_prevmonth")->hide();
    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_nextmonth")->hide();
    QTextCharFormat format;
    format.setForeground(QColor(Res::white));
    ui->calendarWidget->setHeaderTextFormat(format);
    QList<Qt::DayOfWeek> weekdays = {Qt::Monday, Qt::Tuesday, Qt::Wednesday, Qt::Thursday, Qt::Friday};
    for (auto day : weekdays) ui->calendarWidget->setWeekdayTextFormat(day, format);
    format.setForeground(QColor(Res::defaultColor));
    weekdays = {Qt::Saturday, Qt::Sunday};
    for (auto day : weekdays) ui->calendarWidget->setWeekdayTextFormat(day, format);

    // date suffix


    int day = pickedDate.day();
    QString suffix = Res::getSuffix(day);
    QString title = pickedDate.toString("dddd, MMMM d");
    title.replace(QString::number(day), QString::number(day) + suffix);
    ui->todayGroup->setTitle(title + ": tasks");

    // everything else
    ui->tasksListWidget->addItem(Res::noTasksPlaceholders.at(QRandomGenerator::global()->bounded(Res::noTasksPlaceholders.size())));
    data.color = Res::defaultColor;

    ui->todayGroup->setStyleSheet(Res::colorStyle.arg(Res::white));
    ui->newTaskButton->setStyleSheet(Res::colorStyle.arg(Res::white));

    ui->taskTypeBox->addItem(Res::defaultType);
    ui->taskTypeBox->addItem(Res::dueType);
    ui->taskTypeBox->addItem(Res::deadlineType);

    ui->taskRecurrenceBox->addItem(Res::Rec.key(0));
    ui->taskRecurrenceBox->addItem(Res::Rec.key(1));
    ui->taskRecurrenceBox->addItem(Res::Rec.key(7));
    ui->taskRecurrenceBox->addItem(Res::Rec.key(-1));
}

void MainWindow::saveTasks()
{
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
}

void MainWindow::savePreferences()
{
    QSaveFile prefsFile(Res::prefsFileName);
    if (!prefsFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "File Error", "Failed to open tasks file for writing.");
        return;
    }

    QJsonArray tagsArray;
    auto keys = tags.keys();
    for (const QString &tag : keys)
        tagsArray.append(tag);

    QJsonObject obj;
    obj["tags"] = tagsArray;
    QJsonDocument doc(obj);

    prefsFile.write(doc.toJson());
    if (!prefsFile.commit())
        QMessageBox::critical(this, "File Error", "Failed to save tasks.");
}

void MainWindow::loadFiles()
{
    // reading tasks
    QFile tasksFile(Res::tasksFileName);
    if (tasksFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(tasksFile.readAll());
        tasksFile.close();

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(Task(val.toObject()));
    }

    // reading preferences
    QFile prefsFile(Res::prefsFileName);
    if (prefsFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(prefsFile.readAll());
        prefsFile.close();

        QJsonObject obj = doc.object();
        if (obj.contains("tags"))
        {
            QJsonArray tagsArray = obj["tags"].toArray();
            for (const auto &tag : tagsArray)
                tags.insert(tag.toString(), false);
        }
    }
}

// Slots
void MainWindow::onAddTaskButton()
{
    if (ui->taskNameEdit->text().trimmed().isEmpty()) {
        ui->taskNameEdit->setStyleSheet(Res::backgroundStyle.arg(Res::error));
        return;
    }

    QList<QString> currentTags;
    for (auto it = tags.cbegin(); it != tags.cend(); ++it) {
        if (it.value())
            currentTags.append(it.key());
    }

    Task task(
        ui->taskNameEdit->text(),
        currentTags,
        ui->taskTypeBox->currentText(),
        QDateTime(ui->taskDateEdit->date(), ui->taskTimeEdit->time()),
        data.recurrence,
        data.color.name(),
        ui->taskDescriptionEdit->toPlainText(),
        false);

    auto it = std::lower_bound(tasks.begin(), tasks.end(), task,
                               [](const Task &a, const Task &b){ return a.time < b.time; });
    tasks.insert(it, task);

    if (Res::mode == Res::Mode::def) saveTasks();
    changeState(State::default_view);
}

void MainWindow::onRecurrenceBox(const QString& text)
{
    if (text != Res::Rec.key(-1)) {
        data.recurrence = text;
        return;
    }


    // here's the magic
}

void MainWindow::onDateClick(const QDate &date)
{
    pickedDate = date;
    updateDefaultView();
}

void MainWindow::onPickTagsButton()
{
    // QScopedPointer<QDialog> dialog = new QDialog(this);
    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle("Name thy tag");
    QVBoxLayout *layout = new QVBoxLayout(dialog);

    for (auto it = tags.cbegin(); it != tags.cend(); ++it) {
        QCheckBox *checkBox = new QCheckBox(it.key(), dialog);
        checkBox->setChecked(it.value());
        layout->addWidget(checkBox);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    QLineEdit *line = new QLineEdit(dialog);
    QPushButton *button = new QPushButton("Add...", dialog);
    QPushButton *doneButton = new QPushButton("Done", dialog);

    hLayout->addWidget(line);
    hLayout->addWidget(button);
    layout->addLayout(hLayout);
    layout->addWidget(doneButton);

    connect(button, &QPushButton::clicked, [layout, dialog, line, this] {
        QString tag = line->text();
        if (tag == "") return;
        tags.insert(tag, true);
        QCheckBox *checkBox = new QCheckBox(tag, dialog);
        checkBox->setCheckState(Qt::Checked);
        layout->insertWidget(layout->count() - 2, checkBox);
        line->clear();
    });

    connect(doneButton, &QPushButton::clicked, [dialog, this] {
        for (QCheckBox *cb : dialog->findChildren<QCheckBox*>())
            tags[cb->text()] = cb->isChecked();

        dialog->close();
    });

    dialog->exec();
}

void MainWindow::onPickColorButton()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle("Choose task's color");
    dialog.setCurrentColor(QColor(Res::defaultColor));

    dialog.setCustomColor(0, Res::defaultColor);
    dialog.setCustomColor(1, Res::red);
    dialog.setCustomColor(2, Res::yellow);
    dialog.setCustomColor(3, Res::green);
    dialog.setCustomColor(4, Res::white);

    if (dialog.exec() != QDialog::Accepted)
        return;

    QColor color = dialog.selectedColor();
    if (color.isValid()){
        data.color = color;
        ui->taskColorButton->setStyleSheet(Res::colorStyle.arg(data.color.name()));
    }
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

// Helper functions
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

    // here the magic should begin.

    TaskWidget::setDate(pickedDate);
    QVector<TaskWidget*> deadlinesTasks, dueTasks, defaultTasks;
    for (Task &task : tasks)
    {
        if (task.time.date() != pickedDate && task.type != Res::defaultType)
            continue;

        TaskWidget *widget = new TaskWidget(this, &task);

        if (task.type == Res::defaultType)
            defaultTasks.append(widget);
        else if (task.type == Res::dueType)
            dueTasks.append(widget);
        else if (task.type == Res::deadlineType)
            deadlinesTasks.append(widget);
    }

    auto setTasks = [this, layout](const QVector<TaskWidget*> tasksP, const QString type){
        if (tasksP.empty()) return;
        QLabel *label = new QLabel();
        label->setText(type);
        layout->addWidget(label);
        for (TaskWidget* task : tasksP)
            layout->addWidget(task);
    };
    setTasks(dueTasks, "Due tasks");
    setTasks(deadlinesTasks, "Deadlines");
    setTasks(defaultTasks, "Default tasks");

    if (dueTasks.empty() && deadlinesTasks.empty()) {
        QString labelText;
        if (!defaultTasks.empty())
            labelText = ((pickedDate == QDate::currentDate()) ? "No tasks today" : "No tasks this day");
        else
            labelText = "No tasks";

        QLabel *label = new QLabel(labelText);
        label->setStyleSheet(Res::noTasksStyle);
        label->setAlignment(Qt::AlignCenter);
        layout->insertWidget(0, label);
    }

    layout->update();
}

void MainWindow::clearInputWindow()
{
    data.clear();
    for (auto &value : tags) value = false;

    ui->taskNameEdit->setStyleSheet("");
    ui->taskNameEdit->clear();
    ui->taskTypeBox->setCurrentIndex(0);
    ui->taskDateEdit->setDate(QDate::currentDate());
    ui->taskTimeEdit->setTime(QTime::currentTime());
    ui->taskColorButton->setStyleSheet(Res::colorStyle.arg(Res::defaultColor));
    ui->taskDescriptionEdit->clear();
    ui->taskRecurrenceBox->setCurrentIndex(0);
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





















