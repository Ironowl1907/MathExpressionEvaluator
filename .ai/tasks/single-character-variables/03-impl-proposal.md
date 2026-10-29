---
task_name: single-character-variables
source: free text
---

## 1. Design Summary
Add single-character ASCII variable and assignment support to the expression evaluator REPL. `headers/lexer.hpp` and `src/lexer.cpp` tokenize single letters (`a`-`z`, `A`-`Z`) as `Variable` and `=` as `Assign`. `headers/Nodes.hpp` defines `SymbolTable = std::map<char, double>`, `class EvaluationError : public std::exception`, `VariableNode`, and updates AST evaluation to receive `const SymbolTable &`. `headers/parser.hpp` and `src/parser.cpp` return a `Statement` struct from `Parser::Parse`, enforcing `<var> = <expr>` top-level syntax, supporting unary negation `-x`, and verifying no trailing tokens remain. `src/main.cpp` maintains the session symbol table, updates the quit command and banner to `'quit' to quit`, executes assignments only upon successful evaluation, suppresses numeric output on `EvaluationError`, and frees AST memory each iteration.

## 2. Targets

### T1 — Lexer variable and assignment tokens
- **Source**: `01-brief-and-context.md` Acceptance Criterion 1
- **Statement**: Single-character ASCII letters (`a`-`z`, `A`-`Z`) and assignment operator (`=`) are recognized as valid tokens by the lexer.

### T2 — Case-sensitive variable naming
- **Source**: `01-brief-and-context.md` Acceptance Criterion 2
- **Statement**: Variable names are case-sensitive (`a` and `A` refer to distinct variables).

### T3 — Top-level assignment execution and output
- **Source**: `01-brief-and-context.md` Acceptance Criterion 3
- **Statement**: Top-level assignment statements (`<var> = <expr>`) store the evaluated numeric result in a session symbol table and print the assigned value.

### T4 — Variable reassignment
- **Source**: `01-brief-and-context.md` Acceptance Criterion 4
- **Statement**: Variables can be reassigned to new values at any time during the REPL session.

### T5 — Variable references in expressions and unary negation
- **Source**: `01-brief-and-context.md` Acceptance Criterion 5, `02-analysis.md` O3
- **Statement**: Variables can be referenced anywhere numeric values are allowed, including binary arithmetic operations, parenthesized sub-expressions, and unary negation (`-x`).

### T6 — Undefined variable error handling and output suppression
- **Source**: `01-brief-and-context.md` Acceptance Criterion 6, `02-analysis.md` O1
- **Statement**: Evaluating an expression referencing an undefined variable prints `[Evaluation ERROR] Undefined variable '<var>'`, aborts evaluation, suppresses numeric output, and avoids mutating session state or crashing.

### T7 — REPL quit command requiring `quit` or `Quit`
- **Source**: `01-brief-and-context.md` Acceptance Criterion 7
- **Statement**: The REPL quit command requires entering `quit` (or `Quit`), freeing single characters `q` and `Q` for variable use.

### T8 — REPL startup banner update
- **Source**: `02-analysis.md` O2
- **Statement**: The REPL startup banner instructs the user to enter `quit` (or `Quit`) to exit instead of `'Q' to quit`.

### T9 — Prior state preservation on failed assignment
- **Source**: `02-analysis.md` O4
- **Statement**: An assignment statement that fails evaluation due to an undefined variable does not modify or erase any existing variable bindings in the symbol table.

### T10 — Top-level assignment syntax restriction
- **Source**: `02-analysis.md` O5
- **Statement**: The assignment operator `=` is accepted only in top-level statements of the form `<var> = <expr>`, and rejected with a syntax error in all other positions.

### T11 — Per-iteration AST memory deallocation
- **Source**: `02-analysis.md` O6
- **Statement**: All dynamically allocated AST nodes created during expression or assignment parsing are deallocated within each REPL loop iteration.

## 3. Recipe

### S1 — Add Variable and Assign token types
- **Action**: Modify
- **Touches**: `headers/lexer.hpp` (`TokenType`)
- **Contract**: Literal, below
- **Behavior**: Adds `Variable` and `Assign` members to `enum TokenType` before `Error`.
- **Checks**: V1

    ```cpp
    enum TokenType {
      Integer,
      Sum,
      Rest,
      Div,
      Mul,
      OpenPar,
      ClosePar,
      Variable,
      Assign,
      Error
    };
    ```

### S2 — Tokenize single-letter variables and assignment operator
- **Action**: Modify
- **Touches**: `src/lexer.cpp` (`Lexer::Tokenize`)
- **Contract**: None
- **Behavior**: In `src/lexer.cpp`, adds `case '=': Tokens.push_back(token(Assign, "=")); break;` to `Lexer::Tokenize` switch statement. In `default` branch, adds `else if (isalpha(at())) { Tokens.push_back(token(Variable, std::string(1, at()))); }` to recognize ASCII letters as single-character `Variable` tokens.
- **Checks**: V1

### S3 — Define symbol table, evaluation error, VariableNode, and update Node::evaluate
- **Action**: Modify
- **Touches**: `headers/Nodes.hpp` (`SymbolTable`, `EvaluationError`, `Node::evaluate`, `NumberNode::evaluate`, `AddNode::evaluate`, `SubtractNode::evaluate`, `MultiplyNode::evaluate`, `DivideNode::evaluate`, `NegateNode::evaluate`, `VariableNode`)
- **Contract**: Literal, below
- **Behavior**: In `headers/Nodes.hpp`, includes `<exception>`, `<iostream>`, `<map>`. Defines `using SymbolTable = std::map<char, double>;`. Defines `class EvaluationError : public std::exception`. Updates `Node::evaluate` signature to `virtual double evaluate(const SymbolTable &symbols) const = 0;`. Updates `NumberNode`, `AddNode`, `SubtractNode`, `MultiplyNode`, `DivideNode`, and `NegateNode` to take `const SymbolTable &symbols` and pass to child node `evaluate` calls. Defines `class VariableNode : public Node` taking `char name`; in `evaluate(const SymbolTable &symbols) const`, looks up `name` in `symbols`: if found returns value; if not found, writes `[Evaluation ERROR] Undefined variable '<name>'` to `std::cout` followed by newline and throws `EvaluationError()`.
- **Checks**: V1, V2, V5, V6

    `headers/Nodes.hpp` (`SymbolTable`, `EvaluationError`, `VariableNode`, `Node`)
    ```cpp
    #pragma once

    #include <exception>
    #include <iostream>
    #include <map>

    using SymbolTable = std::map<char, double>;

    class EvaluationError : public std::exception {
    public:
      const char *what() const noexcept override {
        return "Evaluation error";
      }
    };

    // Base classes for all node types
    class Node {
    public:
      virtual double evaluate(const SymbolTable &symbols) const = 0;
      virtual ~Node() {}
    };

    class BinaryOperationNode : public Node {
    protected:
      Node *left;
      Node *right;

    public:
      BinaryOperationNode(Node *l, Node *r) : left(l), right(r) {}

      virtual ~BinaryOperationNode() {
        delete left;
        delete right;
      }
    };

    class UnaryOperationNode : public Node {
    protected:
      Node *value;

    public:
      UnaryOperationNode(Node *v) : value(v) {}

      virtual ~UnaryOperationNode() {
        delete value;
      }
    };

    // Number node class
    class NumberNode : public Node {
    private:
      double value;

    public:
      NumberNode(double val) : value(val) {}
      double evaluate(const SymbolTable &symbols) const override { return value; }
    };

    // Variable node class
    class VariableNode : public Node {
    private:
      char name;

    public:
      VariableNode(char n) : name(n) {}
      double evaluate(const SymbolTable &symbols) const override {
        auto it = symbols.find(name);
        if (it == symbols.end()) {
          std::cout << "[Evaluation ERROR] Undefined variable '" << name << "'\n";
          throw EvaluationError();
        }
        return it->second;
      }
    };

    // Add node class
    class AddNode : public BinaryOperationNode {
    public:
      AddNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
      double evaluate(const SymbolTable &symbols) const override {
        return left->evaluate(symbols) + right->evaluate(symbols);
      }
    };

    // Subtract node class
    class SubtractNode : public BinaryOperationNode {
    public:
      SubtractNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
      double evaluate(const SymbolTable &symbols) const override {
        return left->evaluate(symbols) - right->evaluate(symbols);
      }
    };

    // Multiply node class
    class MultiplyNode : public BinaryOperationNode {
    public:
      MultiplyNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
      double evaluate(const SymbolTable &symbols) const override {
        return left->evaluate(symbols) * right->evaluate(symbols);
      }
    };

    // Divide node class
    class DivideNode : public BinaryOperationNode {
    public:
      DivideNode(Node *l, Node *r) : BinaryOperationNode(l, r) {}
      double evaluate(const SymbolTable &symbols) const override {
        return left->evaluate(symbols) / right->evaluate(symbols);
      }
    };

    class NegateNode : public UnaryOperationNode {
    public:
      NegateNode(Node *v) : UnaryOperationNode(v) {}
      double evaluate(const SymbolTable &symbols) const override {
        return value->evaluate(symbols) * (-1);
      }
    };
    ```

### S4 — Declare Statement struct and update Parser interface
- **Action**: Modify
- **Touches**: `headers/parser.hpp` (`Statement`, `Parser::Parse`)
- **Contract**: Literal, below
- **Behavior**: In `headers/parser.hpp`, defines `struct Statement { bool isAssignment = false; char variable = '\0'; Node *node = nullptr; };`. Changes `Parser::Parse` signature to `Statement Parse(std::vector<Token> raw);`.
- **Checks**: V1, V3, V10

    `headers/parser.hpp` (`Statement`, `Parser`)
    ```cpp
    #pragma once

    #include "../headers/Nodes.hpp"
    #include "../headers/lexer.hpp"

    struct Statement {
      bool isAssignment = false;
      char variable = '\0';
      Node *node = nullptr;
    };

    class Parser {
    private:
      unsigned int index = 0;
      std::vector<Token> Input;

    private:
      Token at();
      Token peek();

      Node *ParseExpr();
      Node *ParseFactor();
      Node *ParseTerm();

    public:
      Statement Parse(std::vector<Token> raw);
    };
    ```

### S5 — Implement statement parsing, variable factors, and syntax validation
- **Action**: Modify
- **Touches**: `src/parser.cpp` (`Parser::Parse`, `Parser::ParseFactor`)
- **Contract**: Literal, below
- **Behavior**: In `src/parser.cpp`, implements `Statement Parser::Parse(std::vector<Token> raw)`. If `raw` has at least two tokens with `raw[0].type == Variable` and `raw[1].type == Assign`, sets `isAssignment = true`, sets `variable = raw[0].value[0]`, advances `index = 2`, and parses RHS via `ParseExpr()`. Otherwise, parses expression via `ParseExpr()` with `isAssignment = false`. In both cases, verifies `index == Input.size()`; if unconsumed tokens remain, prints `[Parsing ERROR] Unexpected token '` followed by token value and `' at line: ` followed by `index`, deletes the parsed node, and returns `{false, '\0', nullptr}` (or with assignment flag). In `ParseFactor`, adds support for `at().type == Variable` returning `new VariableNode(temp.value[0])`, and `at().type == Rest && peek().type == Variable` returning `new NegateNode(new VariableNode(temp.value[0]))`.
- **Checks**: V1, V3, V5, V10

    `src/parser.cpp` (`Parser::Parse`, `Parser::ParseFactor`)
    ```cpp
    Statement Parser::Parse(std::vector<Token> raw) {
      Input = raw;
      index = 0;
      if (Input.empty()) {
        return {false, '\0', nullptr};
      }

      if (Input.size() >= 2 && Input[0].type == Variable && Input[1].type == Assign) {
        char varName = Input[0].value[0];
        index = 2;
        Node *expr = ParseExpr();
        if (expr == nullptr) {
          return {true, varName, nullptr};
        }
        if (index < Input.size()) {
          std::cout << "[Parsing ERROR] Unexpected token '" << at().value
                    << "' at line: " << index << '\n';
          delete expr;
          return {true, varName, nullptr};
        }
        return {true, varName, expr};
      }

      Node *par = ParseExpr();
      if (par == nullptr) {
        return {false, '\0', nullptr};
      }
      if (index < Input.size()) {
        std::cout << "[Parsing ERROR] Unexpected token '" << at().value
                  << "' at line: " << index << '\n';
        delete par;
        return {false, '\0', nullptr};
      }
      return {false, '\0', par};
    }
    ```

### S6 — Update REPL loop, symbol table management, banner, and memory cleanup
- **Action**: Modify
- **Touches**: `src/main.cpp` (`solve`, `main`)
- **Contract**: Literal, below
- **Behavior**: In `src/main.cpp`, updates `solve` to `double solve(const Node *tree, const SymbolTable &symbols) { return tree->evaluate(symbols); }`. In `main`, instantiates `SymbolTable symbolTable;`. Updates startup banner to `'quit' to quit`. Updates exit check to `if (input == "Quit" || input == "quit") return 0;`. Updates parser call to `Statement stmt = parser.Parse(TokenList);`. Guards empty or error tokens. Evaluates `stmt.node` in `try { double result = solve(stmt.node, symbolTable); if (stmt.isAssignment) { symbolTable[stmt.variable] = result; } std::cout << result << '\n'; } catch (const EvaluationError &) {}`. Deletes `stmt.node` unconditionally at the end of each loop iteration. Removes outside-loop `delete Tree;`.
- **Checks**: V1, V2, V3, V4, V6, V7, V8, V9, V11

    `src/main.cpp` (`solve`, `main`)
    ```cpp
    #include <iostream>
    #include <string>
    #include <vector>

    #include "../headers/lexer.hpp"
    #include "../headers/parser.hpp"

    double solve(const Node *tree, const SymbolTable &symbols) {
      return tree->evaluate(symbols);
    }

    int main(int argc, char *argv[]) {
      Lexer lexer;
      Parser parser;
      SymbolTable symbolTable;

      std::cout << "'quit' to quit" << '\n';
      while (1) {
        std::cout << "calc >> ";
        std::string input;
        if (!std::getline(std::cin, input))
          break;

        if (input == "Quit" || input == "quit")
          return 0;

        std::vector<Token> TokenList = lexer.Tokenize(input);
        if (TokenList.empty() || TokenList[0].type == Error)
          continue;

        std::cout << "N Tokens: " << TokenList.size() << '\n';

        std::cout << "------------------------" << '\n';

        Statement stmt = parser.Parse(TokenList);
        if (stmt.node == nullptr)
          continue;

        try {
          double result = solve(stmt.node, symbolTable);
          if (stmt.isAssignment) {
            symbolTable[stmt.variable] = result;
          }
          std::cout << result << '\n';
        } catch (const EvaluationError &) {
        }

        delete stmt.node;
      }

      return 0;
    }
    ```

### S7 — Build binary with Makefile
- **Action**: Run
- **Touches**: None
- **Contract**: None
- **Behavior**: Executes `make clear && make` from repository root directory `/home/ironowl/dev/MathExpressionEvaluator`. Compiles object files `main.o`, `lexer.o`, `parser.o` and links executable `./final.out` with zero compiler errors or warnings.
- **Checks**: V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11

## 4. Checks

### V1 — Compilation and binary build verification
- **Method**: In `/home/ironowl/dev/MathExpressionEvaluator`, run `make clear && make`.
- **Expected**: Exits with status 0; generates executable `final.out`.
- **Verifies**: T1

### V2 — Case-sensitive variable assignment and resolution
- **Method**: Run `./final.out` and send stdin: `a = 10\nA = 20\na\nA\nquit\n`.
- **Expected**: Outputs `10` for `a` and `20` for `A`, confirming independent, case-sensitive storage.
- **Verifies**: T2, T3

### V3 — Immediate evaluation output on assignment
- **Method**: Run `./final.out` and send stdin: `x = 10 + 5\nx\nquit\n`.
- **Expected**: Immediate output prints `15` upon assignment line; subsequent evaluation of `x` outputs `15`.
- **Verifies**: T3

### V4 — Variable reassignment across session
- **Method**: Run `./final.out` and send stdin: `x = 5\nx = 20\nx\nquit\n`.
- **Expected**: Final evaluation of `x` outputs `20`.
- **Verifies**: T4

### V5 — Variable expressions and unary negation
- **Method**: Run `./final.out` and send stdin: `x = 4\n2 * (x + 1)\n-x\nquit\n`.
- **Expected**: Expression `2 * (x + 1)` outputs `10`; expression `-x` outputs `-4`.
- **Verifies**: T5

### V6 — Undefined variable error output and suppression
- **Method**: Run `./final.out` and send stdin: `y + 2\nquit\n`.
- **Expected**: Outputs `[Evaluation ERROR] Undefined variable 'y'`; prints no numeric result; process exits cleanly with status 0.
- **Verifies**: T6

### V7 — Quit command update and single-character Q/q variable freedom
- **Method**: Run `./final.out` and send stdin: `q = 7\nq\nquit\n`; then in a second run send stdin: `Q = 9\nQ\nQuit\n`.
- **Expected**: Neither `q` nor `Q` terminates the session; both variables evaluate and print values; session terminates with exit code 0 when `quit` or `Quit` is entered.
- **Verifies**: T7

### V8 — Startup banner text verification
- **Method**: Run `./final.out` with stdin `quit\n` and inspect the first line of output.
- **Expected**: The first stdout line is `'quit' to quit`.
- **Verifies**: T8

### V9 — Prior state preservation on failed assignment
- **Method**: Run `./final.out` and send stdin: `x = 10\nx = y + 1\nx\nquit\n`.
- **Expected**: Emits `[Evaluation ERROR] Undefined variable 'y'`; subsequent evaluation of `x` prints `10`.
- **Verifies**: T9

### V10 — Syntax rejection of inline and chained assignments
- **Method**: Run `./final.out` and send stdin: `1 + (x = 2)\na = b = 3\nquit\n`.
- **Expected**: Emits `[Parsing ERROR]` on both inputs; no variables are stored.
- **Verifies**: T10

### V11 — Per-iteration memory deallocation under Valgrind
- **Method**: In `/home/ironowl/dev/MathExpressionEvaluator`, run `valgrind --leak-check=full --error-exitcode=1 ./final.out` with stdin `x = 5\ny = 10\nx + y\nquit\n`.
- **Expected**: Process exits with status 0; Valgrind reports 0 definitely lost bytes from parser allocations.
- **Verifies**: T11

## 5. Certifications

## 6. Traceability
- T1 — Met by: S1, S2, S7 — Checks: V1
- T2 — Met by: S3, S6, S7 — Checks: V2
- T3 — Met by: S4, S5, S6, S7 — Checks: V2, V3
- T4 — Met by: S6, S7 — Checks: V4
- T5 — Met by: S3, S5, S7 — Checks: V5
- T6 — Met by: S3, S6, S7 — Checks: V6
- T7 — Met by: S6, S7 — Checks: V7
- T8 — Met by: S6, S7 — Checks: V8
- T9 — Met by: S5, S6, S7 — Checks: V9
- T10 — Met by: S5, S7 — Checks: V10
- T11 — Met by: S6, S7 — Checks: V11

## 7. Decision Log
- **Q1** — Assignment statement representation: Option 1. `Parser::Parse` returns `Statement` struct (`bool isAssignment`, `char variable`, `Node *node`). AST expression nodes remain pure with const evaluation. `src/main.cpp` executes assignment and updates symbol table upon successful evaluation.
- **Q2** — Evaluation error signaling mechanism: Option 1. `VariableNode::evaluate` prints `[Evaluation ERROR] Undefined variable '<var>'` and throws `EvaluationError : public std::exception`. Caught by `src/main.cpp` to abort evaluation and suppress numeric output.
- **Q3** — Symbol table container type: Option 1. `using SymbolTable = std::map<char, double>;` defined in `headers/Nodes.hpp`.
- **Q4** — Trailing unconsumed token validation: Option 1. `Parser::Parse` verifies `index == Input.size()`. If trailing tokens remain, emits `[Parsing ERROR] Unexpected token '<value>' at line: <index>`, frees allocated nodes, and returns failure.
- **Q5** — REPL startup banner text: Option 1. `src/main.cpp:15` prints `'quit' to quit` on REPL startup.
