---
task_name: single-character-variables
description: Add single-character variables and assignment support to the mathematical expression evaluator REPL
---

## 1. Task Brief

### Problem Statement
The expression evaluator only supports immediate numerical evaluation. Users cannot store intermediate calculation results or reuse named values, requiring repeated manual re-entry of constants in complex expressions.

### Expected Outcome
Users can define single-character variables in the REPL via `<variable> = <expression>`, evaluate expressions referencing previously assigned variables, reassign variables, and see evaluated results output immediately. Referencing undefined variables reports an evaluation error cleanly without terminating the session.

### Acceptance Criteria
1. Single-character ASCII letters (`a`-`z`, `A`-`Z`) and the assignment operator (`=`) are recognized as valid tokens by the lexer.
2. Variable names are case-sensitive (`a` and `A` refer to distinct variables).
3. Top-level assignment statements (`<var> = <expr>`) store the evaluated numeric result in a session symbol table and print the assigned value.
4. Variables can be reassigned to new values at any time during the REPL session.
5. Variables can be referenced in expressions anywhere numeric values are allowed, including binary arithmetic operations, parenthesized sub-expressions, and unary negation (e.g. `-x`).
6. Evaluating an expression that references an undefined variable prints an error message (`[Evaluation ERROR] Undefined variable '<var>'`) and aborts evaluation without crashing or mutating prior state.
7. The REPL quit command requires entering `quit` (or `Quit`), freeing single characters `q` and `Q` for use as variables.

### Out of Scope
- Multi-character variable names (e.g., `foo = 1`).
- Chained assignments (e.g., `a = b = 3`).
- Inline/embedded assignment within expressions (e.g., `1 + (x = 2)`).
- Persistent storage of variables to disk across REPL restarts.

## 2. Context

### Codebase
- `headers/lexer.hpp` — Defines `TokenType`, `Token`, and `Lexer` class interface; needed for adding variable and assignment token types.
- `src/lexer.cpp` — Implements `Lexer::Tokenize`; needed for tokenizing single-letter variables (`a`-`z`, `A`-`Z`), assignment operator (`=`), and handling unexpected characters.
- `headers/parser.hpp` — Declares `Parser` class interface; needed for parsing variable tokens and assignment statements.
- `src/parser.cpp` — Implements recursive-descent parsing; needed for parsing assignment syntax, variable reference factors, and unary negation on variables (`-x`).
- `headers/Nodes.hpp` — Defines AST node hierarchy and `evaluate()`; needed for representing and evaluating variable reference nodes.
- `src/main.cpp` — Implements REPL interactive loop; needed for managing session symbol table, handling assignment execution, and updating quit command logic.
- `Makefile` — Builds the binary `final.out` from source files using `g++`.
- `README.md` — Project documentation and usage instructions.

### Conventions & Prior Art
- Single-pass lexer in `src/lexer.cpp` — Emits lexical errors to standard output and returns an error token sentinel.
- Recursive-descent parser in `src/parser.cpp` — Traverses tokens via `at()` and `peek()`, prints errors to standard output, and returns `nullptr` on parse failure.
- Object-oriented AST in `headers/Nodes.hpp` — Inherits from `Node` with polymorphic `evaluate() const = 0`.
- Interactive REPL loop in `src/main.cpp` — Prompts with `calc >> `, processes input line-by-line, and recovers gracefully from errors by continuing the loop.

### Related Work
- None

### External References
- None

### Constraints
- Must compile with `g++` via existing `Makefile` using standard C++ Standard Library with no new build systems or third-party dependencies.
- Errors on malformed syntax or undefined variables must be reported cleanly without terminating the REPL session.

## 3. Decision Log
- **Q1** — Task name: Use `single-character-variables`.
- **Q2** — Assignment syntax and lifecycle: REPL assignment statements using `=` (`<var> = <expr>`), persisting in memory across subsequent evaluations.
- **Q3** — Supported characters and case sensitivity: ASCII letters `a`-`z` and `A`-`Z`, case-sensitive.
- **Q4** — REPL exit command collision: Quit REPL only on `quit` (and `Quit`), freeing `q` and `Q` as valid variables.
- **Q5** — Undefined variable handling: Print error message (`[Evaluation ERROR] Undefined variable '<var>'`) and abort evaluation without crashing.
- **Q6** — Assignment expression return value: Print assigned value upon successful assignment.
- **Q7** — Assignment statement scope and nesting: Top-level statements only; no inline or chained assignments.
- **Q8** — Variable reassignment: Allowed any number of times.
- **Q9** — Variable expression support: Evaluated as numbers anywhere numeric operands are valid, including binary operations, parentheses, and unary negation (`-x`).
- **Q10** — Compiler and external constraints: Follow existing `g++` setup and `Makefile` with C++ Standard Library only; do not accommodate any new build system.
- **Q11** — Prior art reference: Treat this task as a fresh start with no prior art.
