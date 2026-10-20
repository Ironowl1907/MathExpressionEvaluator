---
task_name: add-single-char-variables
---

Processed questions: Q16

## 1. Change Summary
<!-- prose · required -->
Fulfilling this task transforms the math expression evaluator from a stateless single-expression calculator into a stateful interactive REPL session that stores case-sensitive single-character variables (`a`–`z`, `A`–`Z`). It extends the lexical contract (`headers/lexer.hpp`, `src/lexer.cpp`) with single-character variable and assignment (`=`) tokens, extends the parser (`headers/parser.hpp`, `src/parser.cpp`) to parse top-level assignments (`<var> = <expr>`), variable factors, and unary variable negation (`-x`), and introduces session-wide variable state and lookup/error semantics across the AST evaluator (`headers/Nodes.hpp`) and REPL loop (`src/main.cpp`).

## 2. Impact Map
<!-- bullet list · required -->
- `src/parser.cpp` — Relationship: origin — Must validate full token consumption and consume closing parenthesis in `-(<expr>)` while freeing partial ASTs on parse errors — Impacts: I1, I3, I5
- `headers/Nodes.hpp` — Relationship: origin — AST evaluation contract must propagate undefined-variable errors without returning fabricated numeric values — Impacts: I2
- `src/main.cpp` — Relationship: downstream — REPL loop must guard against empty token lists, prevent output/state mutation on evaluation errors, and reclaim AST memory each iteration — Impacts: I2, I4, I5

## 3. Impacts
<!-- entries · required · repeat per entry -->

### I1 — Trailing unconsumed tokens silently ignored by `Parser::Parse`
- **Lens**: Behavior and regressions
- **Severity**: High
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/parser.cpp` lines 15–23 (`Parser::Parse` calls `ParseExpr()` and returns `par` without checking if `index < Input.size()`).
- **Consequence**: Inputs with trailing tokens that cannot extend an expression or term—such as implicit multiplication (`2x`, `x(2)`, `(1+2)x`), space-separated variables (`a b`), or chained/nested assignments (`x = y = 1`, `1 + (x = 2)`)—stop parsing at the first token boundary and silently evaluate only the prefix instead of rejecting the input with an error as required by Acceptance Criterion 6.
- **Disposition**: Mitigate via: O1 (Settled in: Q1)

### I2 — Partial variable state mutation or fabricated numeric output when an undefined variable is evaluated
- **Lens**: Data and state
- **Severity**: High
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `headers/Nodes.hpp` line 6 (`virtual double evaluate() const = 0;`) and `src/main.cpp` lines 8 and 35 (`double solve(const Node *tree) { return tree->evaluate(); }` and `std::cout << solve(Tree) << '\n';`).
- **Consequence**: Because `Node::evaluate()` unconditionally returns a `double` and `main.cpp` unconditionally prints `solve(Tree)`, referencing an undefined variable inside a compound expression (`1 + y`) or on the right-hand side of an assignment (`x = y + 1` or `x = x + 1` when `x` is undefined) would print a bogus numeric result or overwrite `x`, violating Acceptance Criterion 5.
- **Disposition**: Mitigate via: O2 (Settled in: Q2)

### I3 — Unterminated `-(expr)` negation bug in `Parser::ParseFactor` breaks compound expressions with `-(...)`
- **Lens**: Behavior and regressions
- **Severity**: Medium
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/parser.cpp` lines 81–90 (`if (at().type == Rest && peek().type == OpenPar)` checks `at().type != ClosePar` at line 84 but omits `index++` before returning `new NegateNode(expr)` at line 89).
- **Consequence**: Once complete token consumption (O1) is enforced or whenever `-(...)` is combined with variables or operators in a larger expression (e.g., `x = -(a + 1) + 2` or `-(x) * 3`), parsing fails because the closing `)` of `-(...)` remains unconsumed.
- **Disposition**: Mitigate via: O3 (Settled in: Q3)

### I4 — Empty or whitespace-only REPL input causes out-of-bounds read in `main.cpp`
- **Lens**: Reliability and failure modes
- **Severity**: Medium
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp` lines 24–25 (`std::vector<Token> TokenList = lexer.Tokenize(input); if (TokenList[0].type == Error) continue;` accesses `TokenList[0]` when `TokenList` is empty).
- **Consequence**: Pressing Enter on an empty or whitespace-only line during a multi-line REPL session reads `TokenList[0]` out of bounds, risking a crash that destroys all stored session variables.
- **Disposition**: Mitigate via: O4 (Settled in: Q4)

### I5 — AST heap memory leaks across multi-iteration REPL sessions and syntax errors
- **Lens**: Reliability and failure modes
- **Severity**: Medium
- **Breaking**: No
- **Condition**: Unconditional
- **Evidence**: `src/main.cpp` lines 21–38 (`Tree` is overwritten each loop iteration without `delete`, and `return 0;` bypasses line 38) and `src/parser.cpp` lines 31–88 (early `return nullptr;` branches do not free partially allocated AST nodes).
- **Consequence**: Across multi-iteration REPL sessions using variables, every evaluated line and every failed parse leaks heap-allocated AST nodes for the lifetime of the process.
- **Disposition**: Mitigate via: O5 (Settled in: Q5)

## 4. Obligations
<!-- entries · optional · repeat per entry -->

### O1 — Complete token consumption
- **Requirements**: Top-level parsing must consume all tokens produced by the lexer for an input line; any input that leaves trailing unconsumed tokens must be rejected with a syntax error message, produce no numeric output, and leave all session variables unchanged.
- **Applies**: Always
- **Verification**: Entering `2x`, `x(2)`, `a b`, `x = y = 1`, or `1 + (x = 2)` in the REPL prints a parsing error message, prints no number, and leaves prior variable values intact.
- **Source**: I1

### O2 — Atomic evaluation and assignment on undefined variables
- **Requirements**: Evaluating any expression or assignment right-hand side that references an undefined variable must report an error and abort before printing any numeric output or modifying any variable binding in the session.
- **Applies**: Always
- **Verification**: Setting `x = 10`, then entering `x = y + 1` or `1 + y` prints an undefined-variable error, prints no numeric result, and subsequent evaluation of `x` still prints `10`.
- **Source**: I2

### O3 — Complete token consumption for parenthesized unary negation
- **Requirements**: Parenthesized unary negation `-(<expr>)` must consume its closing parenthesis token so that both standalone `-(<expr>)` and compound expressions containing `-(<expr>)` parse and evaluate accurately.
- **Applies**: Always
- **Verification**: Setting `a = 3`, then evaluating `-(a)` and `x = -(a + 1) * 2` in the REPL prints `-3` and `-8` respectively without parsing errors.
- **Source**: I3

### O4 — Safe handling of empty or whitespace-only input
- **Requirements**: Entering an empty or whitespace-only line in the REPL must not perform out-of-bounds memory access, crash, print a numeric result, or alter session variable bindings, and must return cleanly to the `calc >> ` prompt.
- **Applies**: Always
- **Verification**: Setting `x = 5`, pressing Enter on an empty line and a whitespace-only line (`   `), and then entering `x` returns cleanly to the prompt and prints `5`.
- **Source**: I4

### O5 — Reclaim AST allocations per REPL iteration and on parse failure
- **Requirements**: All heap-allocated AST nodes created during a REPL iteration—whether evaluation succeeds, fails due to an undefined variable, or aborts during parsing—must be freed before the next iteration begins or when the REPL exits.
- **Applies**: Always
- **Verification**: Running a multi-iteration REPL session covering valid assignments, valid expressions, syntax errors, and undefined-variable errors under a leak checker (`valgrind` or `-fsanitize=leak`) reports zero leaked bytes on exit.
- **Source**: I5

## 5. Decision Log
<!-- bullet list · optional -->
- **Q1** — Candidate Impact C1 (Trailing unconsumed tokens silently ignored by `Parser::Parse`): Confirmed High impact; mitigate via O1.
- **Q2** — Candidate Impact C2 (Partial variable state mutation or fabricated numeric output when an undefined variable is evaluated): Confirmed High impact; mitigate via O2.
- **Q3** — Candidate Impact C3 (Unterminated `-(expr)` negation bug in `Parser::ParseFactor`): Confirmed Medium impact; mitigate via O3.
- **Q4** — Candidate Impact C4 (Empty or whitespace-only REPL input causes out-of-bounds read in `main.cpp`): Confirmed Medium impact; mitigate via O4.
- **Q5** — Candidate Impact C5 (AST heap memory leaks across multi-iteration REPL sessions and syntax errors): Confirmed Medium impact; mitigate via O5.
