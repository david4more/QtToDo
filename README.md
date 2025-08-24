# To-Do List App

A simple Qt Widgets To-Do List app to manage daily tasks efficiently.

## Features

- **Main Window**
  - Split view:
    - **Left pane:** Calendar to switch days  
    - **Right pane:** To-Do List for the selected day
  - Menu bar:
    - **Add Task**: create new tasks with optional deadline (modal dialog)
    - **Settings**: manage completed tasks, theme, and font size

- **New Task Dialog**
  - Enter task name, time and optionally description, color and deadline

- **Data Storage**
  - Tasks stored in memory, with optional save/load in JSON

- **Planned Features**

  - pomodoro timer, tags, color coding, deadlines
  - expected time spent, Pomodoro tracking, completition status, subtasks
  - custom month switching, month label above calendar
  - parsing and conversion from .txt
  - list export
  - time to sleep
  - daily schedule via recurring tasks (bedtime, work, gym, etc.)
  - notifications
  - mobile version, synchronization
  - language processing (naming task "celebrate bd monday" will add the task on monday)

## Technical Stack

- **C++17**, **Qt Widgets (Qt 6.9.1)**, **CMake**, **Git**
