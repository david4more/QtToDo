#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "TaskWidget/taskwidget.h"
#include "Task.h"
#include "Resourses.h"

using namespace Res;
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
    format.setForeground(QColor(Color::white));
    ui->calendarWidget->setHeaderTextFormat(format);
    QList<Qt::DayOfWeek> weekdays = {Qt::Monday, Qt::Tuesday, Qt::Wednesday, Qt::Thursday, Qt::Friday};
    for (auto day : weekdays) ui->calendarWidget->setWeekdayTextFormat(day, format);
    format.setForeground(QColor(Color::def));
    weekdays = {Qt::Saturday, Qt::Sunday};
    for (auto day : weekdays) ui->calendarWidget->setWeekdayTextFormat(day, format);

    // date suffix
    int day = pickedDate.day();
    QString suffix = getSuffix(day);
    QString title = pickedDate.toString("dddd, MMMM d");
    title.replace(QString::number(day), QString::number(day) + suffix);
    ui->todayGroup->setTitle(title + ": tasks");

    // everything else
    ui->tasksListWidget->addItem(noTasksPlaceholders.at(QRandomGenerator::global()->bounded(noTasksPlaceholders.size())));

    ui->todayGroup->setStyleSheet(Style::color.arg(Color::white));
    ui->newTaskButton->setStyleSheet(Style::color.arg(Color::white));

    ui->taskTypeBox->addItem(typeStrings[Type::Default]);
    ui->taskTypeBox->addItem(typeStrings[Type::DueTime]);
    ui->taskTypeBox->addItem(typeStrings[Type::Deadline]);

    ui->taskRecurrenceBox->blockSignals(true);
    ui->taskRecurrenceBox->addItem(recStrings[Rec::None]);
    ui->taskRecurrenceBox->addItem(recStrings[Rec::Daily]);
    ui->taskRecurrenceBox->addItem(recStrings[Rec::Weekly]);
    ui->taskRecurrenceBox->addItem(recStrings[Rec::Monthly]);
    ui->taskRecurrenceBox->addItem(recStrings[Rec::Yearly]);
    ui->taskRecurrenceBox->addItem("Custom...");
    ui->taskRecurrenceBox->blockSignals(false);
}

void MainWindow::saveTasks()
{
    QJsonArray tasksArray;
    for (const Task &t : tasks)
        tasksArray.append(t.toJson());

    QSaveFile file(Res::Files::tasks);
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
    QSaveFile prefsFile(Res::Files::prefs);
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
    QFile tasksFile(Res::Files::tasks);
    if (tasksFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(tasksFile.readAll());
        tasksFile.close();

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(Task(val.toObject()));
    }

    // reading preferences
    QFile prefsFile(Res::Files::prefs);
    if (prefsFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(prefsFile.readAll());
        prefsFile.close();

        QJsonObject obj = doc.object();
        QJsonArray tagsArray = obj["tags"].toArray();
        for (const auto &tag : tagsArray)
            tags.insert(tag.toString(), false);
    }
}

// Slots
void MainWindow::onRecurrenceBox(const QString& text)
{
    // Simple cases handling
    if (text != recStrings[Rec::CustomDays] && text != recStrings[Rec::CustomInterval]) {
        ui->taskRecurrenceBox->setItemText(ui->taskRecurrenceBox->count() - 1, recStrings[Rec::CustomDays]);
        ui->taskRecurrenceBox->setProperty("rec", static_cast<int>(Res::recStrings.key(text)));
        if (text != "None") {
            ui->taskTypeBox->setEnabled(false);
            ui->taskTypeBox->setCurrentText(typeStrings[Type::DueTime]);
        } else
            ui->taskTypeBox->setEnabled(true);
        return;
    }

    // Window creation
    QDialog dialog(this);
    dialog.setWindowTitle("Set recurrence");
    QFormLayout *layout = new QFormLayout(&dialog);

    QCheckBox* intervalBox = new QCheckBox("Interval", &dialog);
    QLineEdit* intervalLine = new QLineEdit(&dialog);
    layout->addRow(intervalBox, intervalLine);

    QCheckBox* daysBox = new QCheckBox("Specific days", &dialog);
    QWidget* daysContainer = new QWidget(&dialog);
    QHBoxLayout *daysLayout = new QHBoxLayout;
    daysContainer->setLayout(daysLayout);
    layout->addRow(daysBox, daysContainer);

    QLocale loc;
    for (Qt::DayOfWeek day = Qt::Monday; day <= Qt::Sunday; day = Qt::DayOfWeek(day + 1)) {
        QCheckBox* cb = new QCheckBox(loc.dayName(day, QLocale::ShortFormat), &dialog);
        cb->setProperty("day", static_cast<int>(day));
        daysLayout->addWidget(cb);
    }
    QPushButton* doneButton = new QPushButton("Done", &dialog);
    layout->addRow(doneButton);

    // handle input
    auto dialogAccept = [&]() {
        ui->taskTypeBox->setEnabled(false);
        ui->taskTypeBox->setCurrentText(typeStrings[Type::DueTime]);
        dialog.accept();
    };
    connect(intervalBox, &QCheckBox::clicked, &dialog, [&]() { daysBox->setChecked(false); });
    connect(daysBox, &QCheckBox::clicked, &dialog, [&]() { intervalBox->setChecked(false); });
    connect(doneButton, &QPushButton::clicked, &dialog, [&]() {
        QString buttonText;
        if (intervalBox->isChecked())
        {
            bool ok = false;
            int recurrence = intervalLine->text().toInt(&ok);
            if (ok){
                auto r = static_cast<Rec>(recurrence);
                if (r == Rec::Daily || r == Rec::Weekly || r == Rec::Monthly || r == Rec::Yearly) {
                    ui->taskRecurrenceBox->setCurrentText(Res::recStrings[r]);
                    ui->taskRecurrenceBox->setProperty("rec", recurrence);
                    if (r == Rec::None)
                        dialog.accept();  // falls into simple cases handling
                    else
                        dialogAccept();
                    return;
                }

                buttonText = "Every " + QString::number(recurrence) + " days";
                ui->taskRecurrenceBox->setProperty("rec", static_cast<int>(Rec::CustomInterval));
                ui->taskRecurrenceBox->setProperty("interval", recurrence);
            }
            else {
                errorStyleTimer(intervalLine);
                return;
            }
        }
        else if (daysBox->isChecked())
        {
            QSet<int> recDays;
            for (QCheckBox *cb : daysContainer->findChildren<QCheckBox*>()) {
                if(cb->isChecked()){
                    recDays.insert(cb->property("day").toInt());
                    buttonText += cb->text() + ", ";
                }
            }
            if (recDays.isEmpty()){
                errorStyleTimer(daysContainer);
                return;
            }
            else
                buttonText.chop(2);

            ui->taskRecurrenceBox->setProperty("rec", static_cast<int>(Rec::CustomDays));
            ui->taskRecurrenceBox->setProperty("days", QVariant::fromValue(recDays));
        }
        else{
            errorStyleTimer(daysBox);
            errorStyleTimer(intervalBox);
            return;
        }
        ui->taskRecurrenceBox->blockSignals(true);
        ui->taskRecurrenceBox->setItemText(ui->taskRecurrenceBox->count() - 1, buttonText);
        ui->taskRecurrenceBox->blockSignals(false);
        dialogAccept();
    });

    if (dialog.exec() == QDialog::Rejected)
        ui->taskRecurrenceBox->setCurrentIndex(0);
}

void MainWindow::onAddTaskButton()
{
    if (ui->taskNameEdit->text().trimmed().isEmpty()) {
        errorStyleTimer(ui->taskNameEdit);
        return;
    }

    QList<QString> currentTags;
    for (auto it = tags.cbegin(); it != tags.cend(); ++it) {
        if (it.value())
            currentTags.append(it.key());
    }

    QColor color;
    QVariant varC = ui->taskColorButton->property("color");
    if (varC.isValid())
        color = QColor(varC.toString());
    else
        color = Color::def;

    Rec rec;
    QVariant varR = ui->taskRecurrenceBox->property("rec");
    if (varR.isValid())
        rec = static_cast<Rec>(varR.toInt());
    else
        rec = Rec::None;

    Task task(
        ui->taskNameEdit->text(),
        currentTags,
        typeStrings.key(ui->taskTypeBox->currentText()),
        QDateTime(ui->taskDateEdit->date(), ui->taskTimeEdit->time()),
        color,
        ui->taskDescriptionEdit->toPlainText(),
        rec);

    QVariant varI = ui->taskRecurrenceBox->property("interval");
    if (varI.isValid())
        task.setRecInterval(varI.toInt());

    QVariant varD = ui->taskRecurrenceBox->property("days");
    if (varD.isValid()) {
        QSet<int> recDays = varD.value<QSet<int>>();
        QSet<Qt::DayOfWeek> days;
        for (int d : recDays)
            days.insert(static_cast<Qt::DayOfWeek>(d));

        task.setRecDays(days);
    }

    auto cmp = [](const Task &a, const Task &b) {
        auto priority = [](const Task &t) {
            if (t.getType() == Type::Default) return 1;
            if (t.isRecursive()) return 2;
            return 3;
        };
        int pa = priority(a), pb = priority(b);
        if (pa != pb) return pa < pb;
        return a.getTime() < b.getTime();
    };

    auto it = std::lower_bound(tasks.begin(), tasks.end(), task, cmp);
    tasks.insert(it, task);

    if (Res::mode == Res::Mode::Default) saveTasks();
    changeState(State::default_view);
}

void MainWindow::onPickTagsButton()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Select tags");
    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    for (auto it = tags.cbegin(); it != tags.cend(); ++it) {
        QCheckBox *checkBox = new QCheckBox(it.key(), &dialog);
        checkBox->setChecked(it.value());
        layout->addWidget(checkBox);
    }

    QHBoxLayout *hLayout = new QHBoxLayout;
    QLineEdit *line = new QLineEdit(&dialog);
    QPushButton *button = new QPushButton("Add...", &dialog);
    QPushButton *doneButton = new QPushButton("Done", &dialog);

    hLayout->addWidget(line);
    hLayout->addWidget(button);
    layout->addLayout(hLayout);
    layout->addWidget(doneButton);

    connect(button, &QPushButton::clicked, [layout, &dialog, line, this] {
        QString tag = line->text();
        if (tag == "") {
            errorStyleTimer(line);
            return;
        }
        tags.insert(tag, true);
        QCheckBox *checkBox = new QCheckBox(tag, &dialog);
        checkBox->setCheckState(Qt::Checked);
        layout->insertWidget(layout->count() - 2, checkBox);
        line->clear();
    });

    connect(doneButton, &QPushButton::clicked, [&dialog, this] {
        for (QCheckBox *cb : dialog.findChildren<QCheckBox*>())
            tags[cb->text()] = cb->isChecked();
        dialog.close();
    });

    dialog.exec();
}

void MainWindow::onPickColorButton()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle("Choose task's color");
    dialog.setCurrentColor(QColor(Color::def));

    dialog.setCustomColor(0, Color::def);
    dialog.setCustomColor(1, Color::red);
    dialog.setCustomColor(2, Color::yellow);
    dialog.setCustomColor(3, Color::green);
    dialog.setCustomColor(4, Color::white);

    if (dialog.exec() != QDialog::Accepted)
        return;

    ui->taskColorButton->setProperty("color", dialog.selectedColor().name());
    ui->taskColorButton->setStyleSheet(Style::color.arg(dialog.selectedColor().name()));
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

    TaskWidget::setDate(pickedDate);
    QVector<TaskWidget*> deadlinesTasks, dueTasks, defaultTasks;
    for (Task &task : tasks)
    {
        bool valid = true;
        switch (task.getRec())
        {
        case Rec::None:
            if (task.getTime().date() != pickedDate && task.getType() != Type::Default)
                valid = false;
            break;
        case Rec::CustomDays:
            if (!task.getRecDays().contains(static_cast<Qt::DayOfWeek>(pickedDate.dayOfWeek())))
                valid = false;
            break;
            // also need to handle monthly and yearly cases
        default:
            if (task.getTime().date().daysTo(pickedDate) % task.getRecInterval() != 0)
                valid = false;
            break;
        }
        if (!valid)
            continue;

        TaskWidget *widget = new TaskWidget(this, &task);

        switch (task.getType()) {
        case Type::Default:
            defaultTasks.append(widget); break;
        case Type::DueTime:
            dueTasks.append(widget); break;
        case Type::Deadline:
            deadlinesTasks.append(widget); break;
        }
    }

    auto setTasks = [this, layout](const QVector<TaskWidget*> tasksP, const QString type) {
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
        label->setStyleSheet(Style::noTasks);
        label->setAlignment(Qt::AlignCenter);
        layout->insertWidget(0, label);
    }

    layout->update();
}

void MainWindow::clearInputWindow()
{
    ui->taskRecurrenceBox->setProperty("rec", QVariant());
    ui->taskRecurrenceBox->setProperty("interval", QVariant());
    ui->taskRecurrenceBox->setProperty("days", QVariant());
    ui->taskColorButton->setProperty("color", QVariant());

    for (auto &value : tags) value = false;

    ui->taskNameEdit->setStyleSheet("");
    ui->taskNameEdit->clear();
    ui->taskTypeBox->setCurrentIndex(0);
    ui->taskDateEdit->setDate(QDate::currentDate());
    ui->taskTimeEdit->setTime(QTime::currentTime());
    ui->taskColorButton->setStyleSheet(Style::color.arg(Color::def));
    ui->taskDescriptionEdit->clear();

    ui->taskRecurrenceBox->setCurrentIndex(0);
    ui->taskTypeBox->setEnabled(true);
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

void MainWindow::errorStyleTimer(QWidget* widget)
{
    QTimer* timer = widget->findChild<QTimer*>("errorTimer");
    if (!timer) {
        timer = new QTimer(widget);
        timer->setObjectName("errorTimer");
        timer->setSingleShot(true);
        connect(timer, &QTimer::timeout, [widget]() {
            if (widget) widget->setStyleSheet("");
        });
    }

    widget->setStyleSheet(Style::background.arg(Color::error));
    timer->stop();
    timer->start(2000);
}



















