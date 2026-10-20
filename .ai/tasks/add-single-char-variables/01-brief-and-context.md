---
task_name: add-single-char-variables
description: Add single-character case-sensitive variables with top-level assignment and expression evaluation across the REPL loop.
status: Completed
---

## 1. Task Brief

### Problem Statement
<!-- prose · required -->
Interactive REPL users currently cannot store intermediate calculation results or reuse named values across loop iterations because the math expression evaluator is stateless between input lines, forcing users to manually re-enter numbers or sub-expressions in subsequent calculations.

### Expected Outcome
<!-- prose · required -->
Users can assign results of mathematical expressions to single-character case-sensitive variables (`a`–`z`, `A`–`Z`) using top-level `<var> = <expression>` syntax in the REPL, and reference those variables (including with unary negation) in subsequent expressions and reassignments throughout the session.

### Acceptance Criteria
<!-- ordered list · required -->
1. Any single ASCII alphabetic character (`a`–`z`, `A`–`Z`) is recognized as a valid variable identifier, and variable names are case-sensitive (`x` and `X` are distinct variables).
2. Entering a top-level assignment `<var> = <expr>` evaluates `<expr>`, binds the resulting `double` value to `<var>` for the current REPL session, and prints the evaluated result.
3. Reassigning an already-defined variable (`x = <new_expr>`, including self-referencing updates like `x = x + 1` when `x` is already defined) overwrites its stored value with the newly evaluated result.
4. Referencing a defined variable inside any arithmetic expression (`+`, `-`, `*`, `/`, parentheses) or with unary negation (`-x`) evaluates using the variable's current stored value.
5. Referencing an undefined variable in an expression or on the right-hand side of an assignment outputs an error message, prints no numeric result, leaves existing variable state unchanged, and continues the REPL loop.
6. Multi-character alphabetic sequences (e.g., `ab`), implicit multiplication (e.g., `2x`, `x(2)`), and chained or nested assignments (e.g., `x = y = 1`, `1 + (x = 2)`) are rejected with an error message and do not crash the REPL.
7. Entering standalone `q`, `Q`, `quit`, or `Quit` continues to exit the REPL immediately, while `q` and `Q` can be assigned and referenced within expressions (e.g., `q = 5`, `q + 1`).

### Out of Scope
<!-- bullet list · optional -->
- Multi-character variable identifiers (e.g., `foo`, `var1`)
- Implicit multiplication (e.g., `2x`, `ab`, `x(y)`)
- Chained or inline/nested assignments (e.g., `x = y = 5`, `2 + (x = 3)`)
- Built-in mathematical constants (e.g., `pi`, `e`)
- User-defined functions
- Persisting variables to disk across separate runs of the program

## 2. Context

### Codebase
<!-- bullet list · required -->
- `headers/lexer.hpp` — Defines `TokenType`, `Token`, and the `Lexer` class interface; relevant for adding variable and assignment token types.
- `src/lexer.cpp` — Implements `Lexer::Tokenize`; relevant for recognizing single-character alphabetic variables (`a`–`z`, `A`–`Z`), the `=` operator, and rejecting invalid multi-character sequences.
- `headers/parser.hpp` — Declares the `Parser` class and recursive-descent methods; relevant for parsing top-level assignments and variable references in expressions.
- `src/parser.cpp` — Implements `Parse`, `ParseExpr`, `ParseTerm`, and `ParseFactor`; relevant for handling top-level `<var> = <expr>`, variable factors, unary negation on variables (`-x`), and rejecting chained/nested assignments or implicit multiplication.
- `headers/Nodes.hpp` — Defines the AST node hierarchy (`Node`, `BinaryOperationNode`, `UnaryOperationNode`, `NumberNode`, `AddNode`, `SubtractNode`, `MultiplyNode`, `DivideNode`, `NegateNode`) and `evaluate()`; relevant for representing and evaluating variable references and/or assignments in the AST.
- `src/main.cpp` — Implements the interactive REPL loop (`calc >> `), quit command handling (`q`/`Q`/`quit`/`Quit`), and calls to `Lexer`, `Parser`, and `solve()`; relevant for maintaining session-wide variable state across loop iterations.
- `Makefile` — Builds `final.out` from `src/main.cpp`, `src/lexer.cpp`, and `src/parser.cpp` using `g++`; relevant if any compilation targets or flags are affected.
- `README.md` — Project overview and usage documentation.

### Conventions & Prior Art
<!-- bullet list · optional -->
- Character-by-character single-pass lexing (`src/lexer.cpp`) — Prints lexical errors to `std::cout` and returns `{token(Error)}` sentinel on invalid input.
- Recursive-descent parsing (`src/parser.cpp`) — Uses `ParseExpr`, `ParseTerm`, and `ParseFactor` hierarchy with `at()` and `peek()` lookahead, printing `[Parsing ERROR] ...` to `std::cout` and returning `nullptr` on syntax errors.
- Header-only AST classes (`headers/Nodes.hpp`) — Concrete AST nodes inherit from `Node`, `BinaryOperationNode`, or `UnaryOperationNode` with virtual destructors deleting child pointers.
- REPL diagnostic output (`src/lexer.cpp`, `src/main.cpp`) — Echoes raw input via `log(Input)` and prints token count (`N Tokens: <count>`) and separator line before evaluation; error conditions recover by continuing the `while (1)` loop without crashing.

### Related Work
<!-- bullet list · optional -->
- None — Standalone task on `main` branch (`3c70eff`).

### External References
<!-- bullet list · optional -->
- None — Standard C++ STL only.

### Constraints
<!-- bullet list · optional -->
- Must compile with `g++` via `Makefile` using only the C++ Standard Library (no third-party dependencies).
- Errors (lexical, parsing, or undefined variable reference) must be reported cleanly without crashing or exiting the interactive REPL loop.

## 3. Decision Log
<!-- bullet list · optional -->
- **Q1** — Task Name (`<task_name>`): Use `add-single-char-variables`.
- **Q2** — Problem and Motivation: Enable REPL users to store and reuse results across loop iterations via single-character variables.
- **Q3** — How Variables Are Defined and Assigned: Explicit assignment syntax (`<var> = <expression>`) in the REPL, persisted in memory for the session.
- **Q4** — Valid Variable Characters & Case Sensitivity: Single ASCII letters (`a`–`z`, `A`–`Z`), case-sensitive (`a` != `A`).
- **Q5** — Adjacent Letters & Implicit Multiplication: Reject multi-letter sequences (`ab`) and implicit multiplication (`2x`) as errors; require explicit operators.
- **Q6** — Scope Boundaries (Out of Scope): Multi-character names, implicit multiplication, chained/nested assignments, built-in constants, functions, and cross-session persistence are out of scope.
- **Q7** — Dependencies & Blockers: None; self-contained task.
- **Q8** — Assignment Grammar (Top-Level vs. Inline/Chained): Top-level `<var> = <expr>` only, printing the assigned value; chained or nested assignments are errors.
- **Q9** — Referencing an Undefined Variable: Report an error message, do not print a numeric result, do not mutate variable state, and continue the REPL loop.
- **Q10** — Unary Negation on Variables: Support unary minus directly on variables (`-x`).
- **Q11** — Standalone `q` / `Q` vs. Quit Command: Preserve standalone `q`, `Q`, `quit`, and `Quit` as the REPL exit command while allowing `q` and `Q` in assignments and compound expressions.
- **Q12** — Codebase Files Involved: All 8 repository files (`headers/lexer.hpp`, `src/lexer.cpp`, `headers/parser.hpp`, `src/parser.cpp`, `headers/Nodes.hpp`, `src/main.cpp`, `Makefile`, `README.md`) are relevant context.
- **Q13** — Conventions & Prior Art: Follow existing single-pass lexer, recursive-descent parser, header-only AST node hierarchy, `std::cout` error reporting with sentinel returns, and REPL diagnostic output conventions.
- **Q14** — Related Work: None.
- **Q15** — External References: None (C++ Standard Library only).
- **Q16** — Constraints: Compile with `g++` via `Makefile` using only standard C++ STL; never crash or exit the REPL on user input errors.
