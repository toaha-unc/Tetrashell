Here’s a polished GitHub README for **Tetrashell** with clean formatting, code blocks, and a professional tone:

---

# Tetrashell

**Tetrashell** is a feature-rich C program that functions as an interactive shell for manipulating and inspecting Tetris quicksave binary files. It provides a suite of commands to modify game data, validate save integrity, rank performance, recover lost saves, and more — all from a custom shell interface.

---

## 🔧 Features

- Modify fields like score, lines, or next piece
- Validate save file integrity
- Upload and rank your score against a leaderboard
- Recover lost quicksaves from a disk image
- View and switch between quicksaves
- Undo previous modifications
- Interactive hex conversion training game
- Enhanced UI elements like improved prompts and pretty outputs

---

## 🚀 Getting Started

Tetrashell requires a valid Tetris quicksave binary file path to function. Once launched, it awaits commands at a `tetrashell>` prompt.

---

## 💻 Supported Commands

### `modify`
Modify a quicksave field (`score`, `lines`, or `next_piece`) to a new value.

```sh
tetrashell> modify score 2039
tetrashell> modify next_piece 2
```

---

### `check`
Check if the current quicksave is legitimate using the `check` program.

```sh
tetrashell> check
```

---

### `rank`
Upload your quicksave to a central leaderboard and view top scores.

- With metric and count:
```sh
tetrashell> rank score 30
```
- With metric only:
```sh
tetrashell> rank lines
```
- With no arguments (defaults to score metric):
```sh
tetrashell> rank
```

---

### `recover`
Recover quicksaves from a disk image and optionally switch to a recovered one.

```sh
tetrashell> recover /playpen/a4/a4_disk.img
```

---

### `switch`
Switch the current quicksave to a new binary file.

```sh
tetrashell> switch ./tetris_quicksave2.bin
```

---

### `help`
Display command descriptions. Use with a command for detailed help.

```sh
tetrashell> help
tetrashell> help modify
```

---

### `info`
Show basic information about the current quicksave (path, score, lines).

```sh
tetrashell> info
```

---

### `undo`
Revert the last `modify` command. Can be used multiple times.

```sh
tetrashell> undo
```

---

### `train`
Launch an interactive hex conversion trainer with adaptive difficulty.

```sh
tetrashell> train
```

---

### `exit`
Exit the Tetrashell program.

```sh
tetrashell> exit
```

---

## ✨ Added Features (from Assignment A5)

- Improved prompt
- `quick-rank` support
- Pretty formatting for `rank` and `recover`
- Enhanced save file switching
- Built-in help system
- Save file info display
- Undo functionality
- Hex conversion training game

---

## 📁 Requirements

- GCC-compatible C compiler
- Valid Tetris quicksave binary file
- Dependencies: `check`, `rank`, `recover` executables (provided)

---

## 📌 Notes

- Switching quicksaves via `switch` or `recover` clears undo history.
- `train` command adjusts difficulty based on performance.
- This program was developed as part of a systems programming project.

---
