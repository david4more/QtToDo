# To-Do List App

A simple Qt Widgets To-Do List app, built with Qt Creator.

## Features

- **Left panel:** Calendar navigator, today's tasks, upcoming deadlines  
- **Default view:** picked day's due tasks and deadlines, plus all default tasks
  - Expand task to read recurrence/completion and read/write description, delete button
- **Pomodoro timer:** configurable work/break duration, number of big break cycle
  - Plays alarm, configurable volume
- **New Task form:** configure task's name, tags, type, recurrence, time, color, and description
  - Recurrence: daily, weekly, monthly, yearly or custom: days (Mon, Wed, Fri) or interval (Every 2 days)
- **Settings form:** configure default color, pomo timer and UI features
  - Tasks and preferences are stored in JSON

## Screenshots

![Default view](https://github.com/user-attachments/assets/ab7f56b3-d753-4354-9070-a52da1c5f293)
![Tasks expanded](https://github.com/user-attachments/assets/ffa6b66b-f90b-4466-84ae-590e15c53379)
![Pomodoro](https://github.com/user-attachments/assets/7fb97fe3-7ab3-447a-90ca-588f93ff526a)
![Settings](https://github.com/user-attachments/assets/e0697689-02e6-4196-9ab2-f1a91332484a)
![Custom recurrence](https://github.com/user-attachments/assets/b4f1b8cd-5e2c-467c-a738-600b29e468f2)

More screenshots: [Issues → Screenshots](https://github.com/david4more/Thinker/issues/1)

## Possible improvements

  - save last picked colors
  - notifications
  - mobile version, synchronization
  - use database instead of .json
  - expected time spent + Pomodoro tracking
  - subtasks
  - parsing and conversion from .txt
  - language processing (naming task "celebrate bd monday" will add the task on next monday)
