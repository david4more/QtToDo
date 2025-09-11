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
    pickedDate = QDate::currentDate();
    loadFiles();
    setupUI();
    changeState(State::default_view);
}

void MainWindow::setupUI()
{
    // left panel
    connect(ui->calendarWidget, &QCalendarWidget::clicked, this, &MainWindow::onDateClick);
    connect(ui->newTaskButton, &QPushButton::clicked, this, &MainWindow::onNewTaskButton);
    connect(ui->pomoButton, &QPushButton::clicked, this, [this](){ if (state != State::pomodoro) changeState(State::pomodoro); else changeState(State::default_view); });

    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_prevmonth")->hide();
    ui->calendarWidget->findChild<QToolButton*>("qt_calendar_nextmonth")->hide();
    QTextCharFormat format;
    format.setForeground(QColor(Color::white));
    ui->calendarWidget->setHeaderTextFormat(format);
    QList<Qt::DayOfWeek> weekdays = {Qt::Monday, Qt::Tuesday, Qt::Wednesday, Qt::Thursday, Qt::Friday};
    for (auto day : weekdays) ui->calendarWidget->setWeekdayTextFormat(day, format);
    format.setForeground(defaultColor);

    updateCalendar();

    ui->todayGroup->setTitle(Res::getFancyDate(pickedDate) + ": tasks");

    ui->todayGroup->setStyleSheet(Style::color.arg(Color::white));
    ui->newTaskButton->setStyleSheet(Style::color.arg(Color::white));


    // new task
    connect(ui->addTaskButton, &QPushButton::clicked, this, &MainWindow::onAddTaskButton);
    connect(ui->taskColorButton, &QPushButton::clicked, this, &MainWindow::onPickColorButton);
    connect(ui->taskTagsButton, &QPushButton::clicked, this, &MainWindow::onPickTagsButton);
    connect(ui->taskRecurrenceBox, &QComboBox::currentTextChanged, this, &MainWindow::onRecurrenceBox);

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


    // settings
    connect(ui->settingsColorButton, &QPushButton::clicked, this, &MainWindow::onSettingsColorButton);
    connect(ui->settingsTasksBox, &QCheckBox::clicked, this, [this](bool checked){ showCompletedTasks = checked; });
    connect(ui->settingsLightBox, &QCheckBox::clicked, this, [this](bool checked){ Res::lightMode = checked; } );
    connect(ui->settingsProfessionalBox, &QCheckBox::clicked, this, [this](bool checked){ Res::professionalMode = checked; } );
    connect(ui->settingsDoneButton, &QPushButton::clicked, this, [this](){ changeState((state == State::settings) ? State::default_view : State::settings); });
    connect(ui->settingsButton, &QToolButton::clicked, this, [this](){ changeState((state == State::settings) ? State::default_view : State::settings); });
    connect(ui->timerVolumeSlider, &QSlider::valueChanged, this, [this](int value){
        pomo.alarmVolume = value / 100.f;
        if (pomo.audioPlayer.playbackState() != QMediaPlayer::PlayingState)
            pomo.audioPlayer.play();
        pomo.audioOutput.setVolume(pomo.alarmVolume);
    });

    ui->settingsTasksBox->setChecked(showCompletedTasks);
    ui->settingsLightBox->setChecked(Res::lightMode);
    ui->settingsProfessionalBox->setChecked(Res::professionalMode);

    ui->settingsColorButton->setStyleSheet(Style::color.arg(defaultColor.name()));

    ui->settingsButton->setFixedSize(QSize(24, 24));
    ui->settingsButton->setIcon(QIcon(":/icons/settings"));
    ui->settingsButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->settingsButton->setAutoRaise(true);
    ui->settingsButton->setStyleSheet("QToolButton { padding: 0px; margin: 0px; } QToolButton:hover { background-color: #606060 }");

    ui->settingsWorkBox->setValue(pomo.work);
    ui->settingsBreakBox->setValue(pomo.rest);
    ui->settingsRestBox->setValue(pomo.bigRestCycle);
    ui->timerVolumeSlider->setValue(static_cast<int>(pomo.alarmVolume * 100));

    // pomodoro timer
    connect(ui->startTimerButton, &QPushButton::clicked, this, &MainWindow::onStartTimerButton);
    connect(&timer, &QTimer::timeout, this, &MainWindow::onTimerTimeout);
    connect(ui->timerTimeButton, &QPushButton::clicked, this, &MainWindow::onTimerTimeButton);

    ui->pomoButton->setFixedSize(QSize(24, 24));
    ui->pomoButton->setIcon(QIcon(":/icons/hourglass"));
    ui->pomoButton->setToolButtonStyle(Qt::ToolButtonIconOnly);
    ui->pomoButton->setAutoRaise(true);
    ui->pomoButton->setStyleSheet("QToolButton { padding: 0px; margin: 0px; } QToolButton:hover { background-color: #606060 }");

    resetTimer();

    pomo.audioPlayer.setAudioOutput(&pomo.audioOutput);
    pomo.audioPlayer.setSource(QUrl("qrc:/sound/alarm"));
    pomo.audioOutput.setVolume(pomo.alarmVolume);
}

void MainWindow::saveTasks()
{
    QJsonArray tasksArray;
    for (const Task *t : tasks)
        tasksArray.append(t->toJson());

    QSaveFile file(File::tasks);
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
    QSaveFile prefsFile(File::prefs);
    if (!prefsFile.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "File Error", "Failed to open preferences file for writing.");
        return;
    }

    QJsonArray tagsArray;
    auto keys = tags.keys();
    for (const QString &tag : keys)
        tagsArray.append(tag);

    QJsonObject obj;
    obj[File::tags] = tagsArray;
    obj[File::completedTasks] = showCompletedTasks;
    obj[File::professionalMode] = professionalMode;
    obj[File::lightMode] = lightMode;
    obj[File::defaultColor] = defaultColor.name();
    obj[File::pomoWork] = pomo.work;
    obj[File::pomoBreak] = pomo.rest;
    obj[File::pomoRestCycle] = pomo.bigRestCycle;
    obj[File::alarmVolume] = std::floor(pomo.alarmVolume * 100.0) / 100.0;
    QJsonDocument doc(obj);

    prefsFile.write(doc.toJson());
    if (!prefsFile.commit())
        QMessageBox::critical(this, "File Error", "Failed to save preferences.");
}

void MainWindow::loadFiles()
{
    // reading tasks
    QFile tasksFile(File::tasks);
    if (tasksFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(tasksFile.readAll());
        tasksFile.close();

        QJsonArray arr = doc.array();
        for (const QJsonValue &val : arr)
            tasks.append(new Task(val.toObject()));
    }

    // reading preferences
    QFile prefsFile(File::prefs);
    if (prefsFile.open(QIODevice::ReadOnly))
    {
        QJsonDocument doc = QJsonDocument::fromJson(prefsFile.readAll());
        prefsFile.close();

        QJsonObject obj = doc.object();
        QJsonArray tagsArray = obj[File::tags].toArray();
        for (const auto &tag : tagsArray)
            tags.insert(tag.toString(), false);

        showCompletedTasks = obj.contains(File::completedTasks) ? obj[File::completedTasks].toBool() : true;
        professionalMode = obj.contains(File::professionalMode) ? obj[File::professionalMode].toBool() : false;
        lightMode = obj.contains(File::lightMode) ? obj[File::lightMode].toBool() : false;
        defaultColor = (obj.contains(File::defaultColor) && QColor(obj[File::defaultColor].toString()).isValid()) ? QColor(obj[File::defaultColor].toString()) : QColor(Res::Color::def);
        pomo.work = (obj.contains(File::pomoWork) ? obj[File::pomoWork].toInt() : 25);
        pomo.rest = (obj.contains(File::pomoBreak) ? obj[File::pomoBreak].toInt() : 5);
        pomo.bigRestCycle = (obj.contains(File::pomoRestCycle) ? obj[File::pomoRestCycle].toInt() : 4);
        pomo.alarmVolume = (obj.contains(File::alarmVolume) ? obj[File::alarmVolume].toDouble() : 0.5f);
    }
}

// Slots
void MainWindow::onTimerTimeButton()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Set timer's current time");

    int initMins = static_cast<int>(pomo.currentTime / 60), initSecs = pomo.currentTime % 60;
    QHBoxLayout *layout = new QHBoxLayout(&dialog);
    QSpinBox *minsBox = new QSpinBox(&dialog);
    QSpinBox *secsBox = new QSpinBox(&dialog);

    minsBox->setRange(0, 59);
    minsBox->setValue(initMins);
    secsBox->setRange(1, 59);
    secsBox->setValue(initSecs);
    layout->addWidget(minsBox);
    layout->addWidget(secsBox);

    QPushButton *doneButton = new QPushButton("Done", &dialog);
    layout->addWidget(doneButton);
    connect(doneButton, &QPushButton::clicked, &dialog, [&](){
        if (minsBox->value() != initMins || secsBox->value() != initSecs){
            pomo.currentTime = (minsBox->value() * 60) + secsBox->value();
            ui->timerTimeButton->setText(pomo.getTime());
        }
        dialog.close(); });

    dialog.exec();
}

void MainWindow::onStartTimerButton()
{
    if (!pomo.timerRunning)
        timer.start(1000);
    else
        timer.stop();

    pomo.timerRunning = !pomo.timerRunning;
    ui->startTimerButton->setText(pomo.timerRunning ? "Stop" : "Start");
}

void MainWindow::onTimerTimeout()
{
    --pomo.currentTime;
    ui->timerTimeButton->setText(pomo.getTime());

    if (pomo.currentTime != 0)
        return;

    pomo.audioPlayer.play();

    bool longBreakAccepted = false;
    if (pomo.isWork) {
        if (pomo.cycle >= pomo.bigRestCycle) {
            QMessageBox messageBox;
            messageBox.setWindowTitle(Text::workEnd);
            messageBox.setText(professionalMode ? "Make a long break?" : Text::randomText(Text::pomoLongBreak));
            QPushButton *yesButton = messageBox.addButton("Yes", QMessageBox::AcceptRole);
            QPushButton *noButton  = messageBox.addButton("Skip", QMessageBox::RejectRole);
            messageBox.exec();
            longBreakAccepted = (messageBox.clickedButton() == yesButton);
        } else
            QMessageBox::information(this, Text::workEnd, professionalMode ? "Time for a break" : Text::randomText(Text::pomoWorkDone));
    } else
        QMessageBox::information(this, Text::breakEnd, professionalMode ? "Time to work" : Text::randomText(Text::pomoBreakDone));

    pomo.audioPlayer.stop();

    ui->timerCycleLabel->setText(pomo.resetTime(longBreakAccepted));
    ui->timerTimeButton->setText(pomo.getTime());
}

void MainWindow::onRecurrenceBox(const QString& text)
{
    // Simple cases handling
    if (recStrings.values().contains(text) && text != recStrings[Res::Rec::CustomDays] && text != recStrings[Res::Rec::CustomInterval]) {
        ui->taskRecurrenceBox->setItemText(ui->taskRecurrenceBox->count() - 1, "Custom...");
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
        ui->taskRecurrenceBox->blockSignals(true);
        ui->taskTypeBox->setCurrentText(typeStrings[Type::DueTime]);
        ui->taskRecurrenceBox->blockSignals(false);
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

            if (recDays.size() == 7) {
                ui->taskRecurrenceBox->setCurrentText(Res::recStrings[Res::Rec::Daily]);
                ui->taskRecurrenceBox->setProperty("rec", static_cast<int>(Res::Rec::Daily));
                dialogAccept();
                return;
            }
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

    if (dialog.exec() == QDialog::Rejected) {
        ui->taskRecurrenceBox->setCurrentIndex(0);
        ui->taskTypeBox->setEnabled(true);
    }
}

void MainWindow::onAddTaskButton()
{
    if (ui->taskNameEdit->text().trimmed().isEmpty() || ui->taskNameEdit->text().size() > 50) {
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
        color = defaultColor;

    Rec rec;
    QVariant varR = ui->taskRecurrenceBox->property("rec");
    if (varR.isValid())
        rec = static_cast<Rec>(varR.toInt());
    else
        rec = Rec::None;

    Task *task = new Task(
        ui->taskNameEdit->text(),
        currentTags,
        typeStrings.key(ui->taskTypeBox->currentText()),
        QDateTime(ui->taskDateEdit->date(), ui->taskTimeEdit->time()),
        color,
        ui->taskDescriptionEdit->toPlainText(),
        rec);

    QVariant varI = ui->taskRecurrenceBox->property("interval");
    if (varI.isValid())
        task->setRecInterval(varI.toInt());

    QVariant varD = ui->taskRecurrenceBox->property("days");
    if (varD.isValid()) {
        QSet<int> recDays = varD.value<QSet<int>>();
        QSet<Qt::DayOfWeek> days;
        for (int d : recDays)
            days.insert(static_cast<Qt::DayOfWeek>(d));

        task->setRecDays(days);
    }

    auto cmp = [](const Task *a, const Task *b) {
        auto priority = [](const Task *t) {
            if (t->getType() == Type::Default) return 1;
            if (t->isRecursive()) return 2;
            return 3;
        };
        int pa = priority(a), pb = priority(b);
        if (pa != pb) return pa < pb;
        return a->getDateTime() < b->getDateTime();
    };

    auto it = std::lower_bound(tasks.begin(), tasks.end(), task, cmp);
    tasks.insert(it, task);

    if (!lightMode) saveTasks();
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
    QPushButton *deleteButton = new QPushButton("Delete checked tags", &dialog);
    QPushButton *doneButton = new QPushButton("Done", &dialog);

    hLayout->addWidget(line);
    hLayout->addWidget(button);
    layout->addLayout(hLayout);
    layout->addWidget(deleteButton);
    layout->addWidget(doneButton);

    connect(button, &QPushButton::clicked, [layout, &dialog, line, this] {
        QString tag = line->text();
        if (tag == "" || tag.size() > 25) {
            errorStyleTimer(line);
            return;
        }
        tags.insert(tag, true);
        QCheckBox *checkBox = new QCheckBox(tag, &dialog);
        checkBox->setCheckState(Qt::Checked);
        layout->insertWidget(layout->count() - 3, checkBox);
        line->clear();
    });

    connect(deleteButton, &QPushButton::clicked, [&dialog, this] {
        for (QCheckBox *cb : dialog.findChildren<QCheckBox*>()){
            if (cb->isChecked()){
                tags.remove(cb->text());
                delete cb;
                dialog.adjustSize();
            }
        }
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
    dialog.setCurrentColor(defaultColor);

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
    default:
        changeState(State::default_view); break;
    }
}

void MainWindow::onSettingsColorButton()
{
    QColorDialog dialog(this);
    dialog.setWindowTitle("Choose default color");
    dialog.setCurrentColor(defaultColor);

    dialog.setCustomColor(0, Color::def);
    dialog.setCustomColor(1, Color::red);
    dialog.setCustomColor(2, Color::yellow);
    dialog.setCustomColor(3, Color::green);
    dialog.setCustomColor(4, Color::white);


    if (dialog.exec() != QDialog::Accepted)
        return;

    defaultColor = dialog.selectedColor();
    ui->settingsColorButton->setStyleSheet(Style::color.arg(defaultColor.name()));

    updateCalendar();
}

// Helper functions
void MainWindow::updateLeftPanel()
{
    ui->tasksListWidget->clear();
    ui->deadlinesListWidget->clear();
    for (const auto task : tasks) {
        if (task->getType() == Type::DueTime && task->getDateTime().date() == QDate::currentDate())
                ui->tasksListWidget->addItem(task->getName());
        else if (task->getType() == Type::Deadline && ui->deadlinesListWidget->count() < 8) {
            auto currentDate = QDate::currentDate();
            auto taskDate = task->getDateTime().date();
            if (currentDate <= taskDate && currentDate.daysTo(taskDate) <= 90)
                ui->deadlinesListWidget->addItem(task->getName() + ", " + task->getDateTime().toString("dd-MM HH:mm"));
        }
    }
    if (ui->tasksListWidget->count() == 0)
        ui->tasksListWidget->addItem(professionalMode ? "No tasks today" : Text::randomText(Text::noTasksPlaceholders));

    if (ui->deadlinesListWidget->count() == 0)
        ui->deadlinesListWidget->addItem(professionalMode ? "No deadlines within three months" : Text::randomText(Text::noTasksPlaceholders));
}

void MainWindow::updateDefaultView()
{
    updateLeftPanel();

    QWidget *container = new QWidget;
    QVBoxLayout *layout = new QVBoxLayout(container);
    layout->setAlignment(Qt::AlignTop);

    while (QLayoutItem* child = layout->takeAt(0)) {
        if (QWidget* w = child->widget())
            delete w;
        delete child;
    }

    TaskWidget::setDate(pickedDate);
    QVector<TaskWidget*> deadlinesTasks, dueTasks, defaultTasks;
    for (Task *task : tasks)
    {
        bool valid;
        switch (task->getRec())
        {
        case Rec::None:
            valid = (task->getDateTime().date() == pickedDate || task->getType() == Type::Default); break;
        case Rec::Daily:
            valid = true; break;
        case Rec::Weekly:
            valid = (task->getDateTime().date().dayOfWeek() == pickedDate.dayOfWeek()); break;
        case Rec::Monthly:
            valid = (task->getDateTime().date().day() == pickedDate.day()); break;
        case Rec::Yearly:
            valid = (task->getDateTime().date().dayOfYear() == pickedDate.dayOfYear()); break;
        case Rec::CustomDays:
            valid = (task->getRecDays().contains(static_cast<Qt::DayOfWeek>(pickedDate.dayOfWeek()))); break;
        case Rec::CustomInterval:
            valid = (task->getDateTime().date().daysTo(pickedDate) % task->getRecInterval() == 0); break;
        }
        if (valid && !showCompletedTasks && task->isCompleted(pickedDate))
            valid = false;

        if (!valid)
            continue;

        TaskWidget *widget = new TaskWidget(this, task);
        connect(widget, &TaskWidget::deleteTask, this, [this](Task* task){ tasks.removeOne(task); delete task; updateDefaultView(); });

        switch (task->getType()) {
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

    QWidget *old = ui->scrollArea->takeWidget();
    ui->scrollArea->setWidget(container);
    old->deleteLater();
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
    ui->taskColorButton->setStyleSheet(Style::color.arg(defaultColor.name()));
    ui->taskDescriptionEdit->clear();

    ui->taskRecurrenceBox->setCurrentIndex(0);
    ui->taskTypeBox->setEnabled(true);
}

void MainWindow::changeState(State state)
{
    if (this->state == State::settings){
        if (
            pomo.work != ui->settingsWorkBox->value() ||
            pomo.rest != ui->settingsBreakBox->value() ||
            pomo.bigRestCycle != ui->settingsRestBox->value())
        {
            pomo.work = ui->settingsWorkBox->value();
            pomo.rest = ui->settingsBreakBox->value();
            pomo.bigRestCycle = ui->settingsRestBox->value();
            resetTimer();
        }
        pomo.audioPlayer.stop();
    }

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
    case State::settings:
        ui->newTaskButton->setText("Default View");
        break;
    case State::pomodoro:
        ui->newTaskButton->setText("Default View");
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

void MainWindow::resetTimer()
{
    timer.stop();
    pomo.init();
    ui->startTimerButton->setText("Start");
    ui->timerTimeButton->setText(pomo.getTime());
    ui->timerCycleLabel->setText(professionalMode ? "First cycle: work" : Text::randomText(Text::pomoFirstCycle));
}

void MainWindow::updateCalendar()
{
    QTextCharFormat format;
    format.setForeground(defaultColor);
    for (auto day : {Qt::Saturday, Qt::Sunday}) ui->calendarWidget->setWeekdayTextFormat(day, format);
}


















