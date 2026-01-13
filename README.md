# MiniDB – C Final Project

A simplified **Relational Database Management System (SGBDR)** written entirely in **C**, implementing core database features such as table creation, insertion, selection, deletion, JOIN operations, and on-disk persistence.

This project was developed as the final C assignment for **ESGI 2A3 (2025–2026)** by  
**Minh Cat DO**, **Paco ROBINET-CAMPOY**, and **Cheikh Ahmadou Bamba CISSE**.

---

## Table of Contents

1. Project Description  
2. Pre-requisites  
3. Installation  
4. Features  
5. Usage  
6. Technical Choices  
7. Contributors

---

## 1. Project Description

MiniDB is a simulation of a lightweight relational database engine.  
It supports a SQL-like commands, stored in memory through:

- Linked Lists (tables, columns, rows)
- A Hash Table Index (O(1) average lookup for Primary Keys and Unique columns)
- Dynamic memory management with strict malloc/free discipline
- Binary file import/export for data persistence
- Sort-Merge JOIN implementation for inter-table relationships

This engine is designed to simulate  the behavior of a minimal SGBD while preserving internal clarity, memory safety, and performance.

---

## 2. Pre-requisites

To build and run the project:

- Windows OS (required for the provided Makefile)
- gcc
- mingw32-make.exe

---

## 3. Installation

```bash
git clone https://github.com/DoMinhCat/MiniDB.git
cd MiniDB/
mingw32-make clean
mingw32-make
./MiniDB.exe
```

## 4. Features

### General

- No need to wrap string values in quotes (`Alice` instead of `"Alice"`).
- Table and column names are case-sensitive.
- Supported types: `int`, `string`, `double`.
- Supports executing multiple commands, separated by newlines.
- A command chain must end with a semicolon `;`.

### Insert & Data Integrity

- Values must follow the order of column creation.
- All columns must be filled during insertion.
- INT primary keys support auto-incrementation.
- Primary and foreign keys must be int or string (int > 0).
- UNIQUE and PRIMARY KEY indexed via hash table.
- All foreign keys behave as ON DELETE RESTRICT.

### Select

- Basic projection and filtering supported.
- JOIN implemented using Sort-Merge:
    - For ambiguous column names shared by both tables, the first table wins.
    - When combining JOIN and WHERE, the WHERE column belongs to the first table.
- AND / OR not supported yet.

### Architecture Limitations

- Tables cannot be altered once created.
- Row updates not supported - delete & reinsert instead.
- NOT NULL/DEFAULT not implemented.
- Only one primary key per table - no many-to-many relationship yet.
- Drop order matters due to FK constraints.

### File Import

- Import files must be located in the current directory.

### Internal Limits

- Max columns per table: 50
- Max string length: 256 chars
- Max tables: 100
- Max length for table/column names: 100
- Row count warning at 10,000 rows (increase of execution time)

### Program exit

- Use `quit;` or `exit;`

## 5. Usage

- `create table tab1 ( col1 int pk, col2 string fk references tab_to_refer col_to_refer, col3 double );`  
    Spaces between parentheses are required for correct parsing.

- `show tables;`

- `describe table tab1;`

- `insert into tab1 ( col1, col2 ) values ( val1, val2 );`

- `delete from tab1 [where col1 = 6];`

- `drop table tab1 [, tab2, tab3,...];`

- `select ( col1, col2, col3 ) from tab1 [join tab2 on col1 = col2] [where col1 = xyz];`  
  Order of col1 and col2 in JOIN must match the order of tab1 and tab2 passed in the command 

---

## 6. Technical choice clarification

MiniDB relies entirely on low-level C data structures:

- Tables, rows, and columns implemented as **linked lists**
- **Hash table** with chaining for PRIMARY KEY and UNIQUE indexing
- Query parser fills a compact `Query` struct using a **union**
- JOIN implemented using **Sort-Merge join**, optimized for linked lists
- All operations performed **in-place** through pointer manipulation
- Manual memory management with a dedicated cleanup module
- Binary file import/export for full database persistence

These design choices prioritize clarity, determinism, and performance within project constraints.

---

## 7. Contributors

- Minh Cat DO  
- Paco ROBINET-CAMPOY  
- Cheikh Ahmadou Bamba CISSE

*ESGI Paris 2025 - 2026*
