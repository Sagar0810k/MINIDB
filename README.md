# MINIDB 🗄️

![Visitor Count](https://visitor-badge.laobi.icu/badge?page_id=Sagar0810k.MINIDB)
![Language](https://img.shields.io/badge/language-C-blue.svg)
![Build](https://img.shields.io/badge/build-passing-brightgreen.svg)
![Platform](https://img.shields.io/badge/platform-Windows-lightgrey.svg)

A lightweight, mini relational **Database Management System (DBMS)** built from scratch in **C**, with a custom lexer and parser powered by **Flex** and **Bison**.

---

## 📌 Table of Contents

- [About](#-about)
- [Features](#-features)
- [Project Structure](#-project-structure)
- [Tech Stack](#-tech-stack)
- [Prerequisites](#-prerequisites)
- [Build Instructions](#-build-instructions)
- [Usage](#-usage)
- [How It Works](#-how-it-works)
- [Contributing](#-contributing)
- [Author](#-author)

---

## 📖 About

**MINIDB** is a command-line database engine built as a systems programming project. It implements a subset of SQL, including the ability to create tables, insert records, and query data — all managed through a hand-crafted parser and a file-backed storage engine written in pure C.

This project demonstrates low-level concepts such as lexical analysis, parsing, table storage, and query execution without relying on any third-party database libraries.

---

## ✨ Features

- Custom SQL-like query parser using **Flex** (lexer) and **Bison** (parser)
- Support for basic DDL and DML operations (CREATE TABLE, INSERT, SELECT)
- File-backed persistent storage engine
- Modular architecture with clean separation between lexer, parser, executor, and storage layers
- Windows-native build script (`build.bat`)
- Lightweight — no external database dependencies

---

## 📁 Project Structure

```
MINIDB/
├── include/           # Header files
├── src/
│   ├── main.c         # Entry point
│   ├── db.c           # Core database logic
│   ├── table.c        # Table management
│   ├── storage.c      # File-backed storage engine
│   ├── executor.c     # Query execution engine
│   ├── utils.c        # Utility functions
│   ├── lexer.l        # Flex lexer definition
│   └── parser.y       # Bison grammar/parser definition
├── build/             # Compiled object files and executable (auto-generated)
├── build.bat          # Windows build script
└── README.md
```

---

## 🛠️ Tech Stack

| Component      | Technology          |
|----------------|---------------------|
| Language       | C                   |
| Lexer          | Flex (Lex)          |
| Parser         | Bison (YACC)        |
| Build System   | Batch Script (Windows) |
| Compiler       | GCC                 |

---

## ✅ Prerequisites

Make sure the following tools are installed and available in your system `PATH`:

- **GCC** — C compiler (e.g., via [MinGW-w64](https://www.mingw-w64.org/) on Windows)
- **Flex** — Lexical analyser generator
- **Bison** — Parser generator (GNU Bison)

You can verify they are available by running:

```bash
gcc --version
flex --version
bison --version
```

---

## 🔨 Build Instructions

### Windows

Clone the repository and run the build script:

```bash
git clone https://github.com/Sagar0810k/MINIDB.git
cd MINIDB
build.bat
```

The build script will:

1. Clean any previously generated files
2. Generate the parser using Bison (`parser.y` → `parser.tab.c / .h`)
3. Generate the lexer using Flex (`lexer.l` → `lex.yy.c`)
4. Compile all source files individually
5. Link everything into a final executable: `build/minidb.exe`

---

## ▶️ Usage

After a successful build, run the executable from the project root:

```bash
build\minidb.exe
```

You will be presented with a prompt where you can enter SQL-like commands, for example:

```sql
CREATE TABLE students (id INT, name TEXT, age INT);
INSERT INTO students VALUES (1, 'Alice', 21);
SELECT * FROM students;
```

---

## ⚙️ How It Works

MINIDB is structured as a pipeline:

```
User Input
    │
    ▼
Lexer (Flex)         ← Tokenizes raw SQL text
    │
    ▼
Parser (Bison)       ← Validates grammar, builds AST
    │
    ▼
Executor             ← Interprets and executes commands
    │
    ▼
Storage Engine       ← Reads/writes table data to disk
```

Each layer is independently compiled and linked together. The `include/` directory contains shared header files that define data structures used across all modules.

---

## 🤝 Contributing

Contributions, suggestions, and bug reports are welcome!

1. Fork the repository
2. Create a new branch (`git checkout -b feature/your-feature`)
3. Commit your changes (`git commit -m 'Add some feature'`)
4. Push to the branch (`git push origin feature/your-feature`)
5. Open a Pull Request

---

## 👤 Author

**Sagar Singh Rawat**  
GitHub: [@Sagar0810k](https://github.com/Sagar0810k)

---

> Built with 💻 and a lot of `segmentation fault` debugging.
