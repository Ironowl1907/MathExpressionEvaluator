---
task_name: single-character-variables
---

## 1. Change Summary
Task introduces single-character ASCII variable tokens and top-level assignment statements (`<var> = <expr>`) to REPL. It adds an in-memory session symbol table to store and resolve variable values during expression evaluation. It updates REPL quit handling to require `quit` or `Quit`, freeing single letters `q` and `Q` for variable use. It modifies factor parsing to support unary negation on variables and enforces clean evaluation aborts without state mutation when undefined variables are referenced.

## 2. Impact Map
- `headers/lexer.hpp` — Relationship: origin — Defines token types for single-character variables and assignment operator — Impacts: I6
- `src/lexer.cpp` — Relationship: origin — Tokenizes single-letter ASCII characters as variable tokens and recognizes assignment operator '=' — Impacts: I6
- `headers/parser.hpp` — Relationship: origin — Declares parser interface for variable factors and top-level assignment statements — Impacts: I4, I6
- `src/parser.cpp` — Relationship: origin — Parses variable factor tokens, handles unary negation on variables, and enforces top-level assignment grammar — Impacts: I4, I6
- `headers/Nodes.hpp` — Relationship: upstream — Declares AST node hierarchy and evaluation contract for variable lookup and error signaling — Impacts: I1
- `src/main.cpp` — Relationship: downstream — Manages session symbol table, updates quit command, suppresses numeric output on evaluation errors, and frees AST memory — Impacts: I1, I2, I3, I5, I7

## 3. Impacts

### I1 — Undefined variable evaluation output suppression
- **Lens**: Contracts and interfaces
- **Severity**: High
- **Breaking**: Yes
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp:35` calls `std::cout << solve(Tree) << '\n';` unconditionally whenever parsing succeeds; `headers/Nodes.hpp:6` defines `evaluate()` returning `double` without error signaling.
- **Consequence**: If an expression references an undefined variable, returning a dummy numeric value causes `main.cpp` to print a number after the error message, violating Acceptance Criterion 6.
- **Disposition**: Mitigate via: O1 (Settled in: Q1)

### I2 — REPL startup banner text
- **Lens**: User experience
- **Severity**: High
- **Breaking**: Yes
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp:15` prints `'Q' to quit` on startup; Acceptance Criterion 7 requires typing `quit` (or `Quit`) to exit.
- **Consequence**: Users following the startup banner and typing `Q` will not exit the REPL, but will trigger variable evaluation and undefined variable errors.
- **Disposition**: Mitigate via: O2 (Settled in: Q2)

### I3 — REPL single-letter quit regression
- **Lens**: Contracts and interfaces
- **Severity**: High
- **Breaking**: Yes
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp:21` exits immediately when input is `q` or `Q`.
- **Consequence**: External scripts or users relying on single letters `q` or `Q` to exit the session will no longer terminate the program.
- **Disposition**: Accept: Intended breaking change mandated by Acceptance Criterion 7 and Decision Log Q4 to free letters for variable naming (Settled in: Q3)

### I4 — Variable unary negation parsing
- **Lens**: Behavior and regressions
- **Severity**: High
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/parser.cpp:74-90` only parses unary minus when followed immediately by `Integer` or `OpenPar`.
- **Consequence**: Expressions referencing negated variables such as `-x` fail with syntax errors, violating Acceptance Criterion 5.
- **Disposition**: Mitigate via: O3 (Settled in: Q4)

### I5 — Prior state preservation on failed assignment
- **Lens**: Data and state
- **Severity**: High
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: Acceptance Criteria 3, 4, and 6 require assignments to persist only on success and errors to abort without mutating prior state.
- **Consequence**: Updating symbol table before right-hand side evaluation completes allows failed expressions to corrupt or erase existing variable values.
- **Disposition**: Mitigate via: O4 (Settled in: Q5)

### I6 — Assignment operator scope enforcement
- **Lens**: Contracts and interfaces
- **Severity**: High
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `01-brief-and-context.md` Out of Scope explicitly excludes chained assignments and inline assignments.
- **Consequence**: Treating `=` as a general expression operator would allow invalid inline or chained assignments with unintended semantics.
- **Disposition**: Mitigate via: O5 (Settled in: Q6)

### I7 — REPL loop AST node memory leak
- **Lens**: Reliability and failure modes
- **Severity**: Low
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp:38` deletes `Tree` only after the `while(1)` loop terminates; dynamic allocations in parser accumulate each iteration.
- **Consequence**: Interactive sessions with many assignments and evaluations leak AST heap memory continuously.
- **Disposition**: Mitigate via: O6 (Settled in: Q7)

## 4. Obligations

### O1 — Evaluation error output suppression
- **Requirements**: Evaluating an expression that references an undefined variable must report `[Evaluation ERROR] Undefined variable '<var>'` and must not print any numeric result.
- **Applies**: Always
- **Verification**: Evaluate an expression referencing an undefined variable in the REPL; observe the error message without any subsequent numeric value.
- **Source**: I1

### O2 — REPL exit banner instruction
- **Requirements**: The REPL startup banner must instruct the user to enter `quit` (or `Quit`) to exit instead of `'Q' to quit`.
- **Applies**: Always
- **Verification**: Run `final.out` and observe the startup message instructing `quit` or `Quit`.
- **Source**: I2

### O3 — Variable unary negation parsing
- **Requirements**: The parser must recognize and parse unary minus preceding variable tokens in factor expressions.
- **Applies**: Always
- **Verification**: Assign `x = 5`, evaluate `-x`, and verify the output evaluates to `-5`.
- **Source**: I4

### O4 — Prior state preservation on failed assignment
- **Requirements**: An assignment statement that fails evaluation due to an undefined variable must not modify or erase any existing variable bindings in the symbol table.
- **Applies**: Always
- **Verification**: Assign `x = 10`, execute `x = 5 + y` where `y` is undefined, then evaluate `x` and verify it still outputs `10`.
- **Source**: I5

### O5 — Top-level assignment syntax restriction
- **Requirements**: The assignment operator `=` must only be accepted in top-level statements of the form `<var> = <expr>`, and rejected with a syntax error in all other positions.
- **Applies**: Always
- **Verification**: Enter expressions with inline assignment `1 + (x = 2)` and chained assignment `a = b = 3`; verify syntax errors are emitted and no variables are bound.
- **Source**: I6

### O6 — Per-iteration AST memory deallocation
- **Requirements**: All dynamically allocated AST nodes created during expression or assignment parsing must be deallocated within each REPL loop iteration.
- **Applies**: Always
- **Verification**: Run interactive session under Valgrind or address sanitizer across repeated commands; verify zero heap leaks across loop iterations.
- **Source**: I7

## 5. Decision Log
- **Q1** — Undefined variable output suppression: Mitigate via O1. Abort evaluation without printing numeric result when undefined variable error occurs.
- **Q2** — REPL startup banner text: Mitigate via O2. Update startup banner to instruct entering `quit` (or `Quit`) instead of `'Q' to quit`.
- **Q3** — REPL single-letter quit regression: Accept breaking change. Repurposing `q` and `Q` as valid single-character variables is intended per Acceptance Criterion 7.
- **Q4** — Variable unary negation parsing: Mitigate via O3. Parse unary minus preceding variable tokens in factor parsing.
- **Q5** — Prior state preservation on failed assignment: Mitigate via O4. Update symbol table only after right-hand side expression successfully evaluates.
- **Q6** — Assignment operator scope enforcement: Mitigate via O5. Restrict `=` to top-level statements and reject chained or inline assignments with syntax errors.
- **Q7** — REPL loop AST node memory leak: Mitigate via O6. Deallocate AST nodes allocated during parsing within each REPL loop iteration.
