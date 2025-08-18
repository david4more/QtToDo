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
  - Enter task name, time, and description
  - Completion status

- **Data Storage**
  - Tasks stored in memory, with optional save/load in JSON or todo.txt format

- **Planned Features**

  - pomodoro timer, recurring tasks, tags, color coding, deadlines, 
  - expected time spent, Pomodoro tracking
  - custom month switching, month label above calendar

## Technical Stack

- **C++17**, **Qt Widgets (Qt 6.9.1)**, **CMake**, **Git**
