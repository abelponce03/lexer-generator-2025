# ⚙️ Lexer Generator

![Language](https://img.shields.io/badge/Language-C++-00599C?logo=cplusplus)
![Parser](https://img.shields.io/badge/Parser-Bison%2FYacc-orange)
![Status](https://img.shields.io/badge/Status-Complete-brightgreen)

A **configurable lexer generator** built in C++. It parses regular expression specifications via a Bison/Yacc parser, constructs NFAs from regex ASTs, converts them to DFAs through subset construction, and produces tokenizers that follow **maximal munch** semantics. Includes a comprehensive test suite.

---

## 📑 Table of Contents

- [Features](#features)
- [Tech Stack](#tech-stack)
- [Project Structure](#project-structure)
- [How to Build & Run](#how-to-build--run)
- [How It Works](#how-it-works)
- [Academic Context](#academic-context)

---

## ✨ Features

- **Regex-to-NFA Construction** — Builds Thompson NFAs from parsed regular expression ASTs
- **NFA → DFA Conversion** — Implements the subset construction algorithm for deterministic automata
- **Maximal Munch Tokenization** — Produces tokenizers that always match the longest possible token
- **Bison/Yacc Parser** — Robust grammar-driven parsing of regex specifications
- **Abstract Syntax Tree (AST)** — Clean intermediate representation for regex patterns
- **Comprehensive Test Suite** — Extensive tests validating correct lexer generation and tokenization

---

## 🛠 Tech Stack

| Component       | Technology          |
|-----------------|---------------------|
| Language         | C++ (C++17)        |
| Parser Generator | Bison / Yacc       |
| Build System     | Make / CMake       |
| Testing          | Custom test suite  |

---

## 📁 Project Structure

```
lexer-generator-2025/
├── ast/            # Abstract Syntax Tree node definitions for regex
├── automata/       # NFA and DFA data structures and algorithms
├── regex/          # Regex parsing and pattern handling
├── lexer/          # Lexer/tokenizer generation logic
└── tests/          # Comprehensive test suite
```

---

## 🚀 How to Build & Run

### Prerequisites

- C++17-compatible compiler (GCC ≥ 7 or Clang ≥ 5)
- Bison / Yacc
- Flex (if used for bootstrapping)
- Make or CMake

### Build

```bash
make        # or cmake --build .
```

### Run

```bash
./lexer_generator <spec_file>
```

Provide a specification file with token definitions (regex patterns) and the generator will produce a tokenizer.

### Run Tests

```bash
make test   # or run the test binary directly
```

---

## ⚙️ How It Works

```
Regex Spec → [Bison Parser] → AST → [Thompson Construction] → NFA → [Subset Construction] → DFA → Tokenizer
```

1. **Parsing** — A Bison/Yacc parser reads regex specifications and builds an AST for each token pattern.
2. **NFA Construction** — Each regex AST is converted to a Non-deterministic Finite Automaton using Thompson's construction.
3. **NFA → DFA** — The combined NFA is converted to a Deterministic Finite Automaton via the subset construction algorithm.
4. **Tokenizer Generation** — The DFA is used to produce a tokenizer that scans input text, applying maximal munch semantics to select the longest matching token at each position.

---

## 🎓 Academic Context

> **Course Project** — Compilation, University of Havana, 2025
