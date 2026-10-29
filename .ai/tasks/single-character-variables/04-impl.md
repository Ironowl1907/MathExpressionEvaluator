---
task_name: single-character-variables
---

## 1. Implementation Summary
Added single-character variable support and assignment statements to expression evaluator REPL. Touched headers/lexer.hpp, src/lexer.cpp, headers/Nodes.hpp, headers/parser.hpp, src/parser.cpp, and src/main.cpp. All 11 verification checks V1 to V11 passed, including case-sensitivity, error handling, syntax rejection, and Valgrind memory leak checks.

## 2. Executed Steps

### S1 — Add Variable and Assign token types
- **Action**: Modify
- **Touches**: headers/lexer.hpp (TokenType)
- **Status**: Done
- **Outcome**: Added Variable and Assign enum values to TokenType
- **Checks**: V1

### S2 — Tokenize single-letter variables and assignment operator
- **Action**: Modify
- **Touches**: src/lexer.cpp (Lexer::Tokenize)
- **Status**: Done
- **Outcome**: Added '=' tokenization as Assign and isalpha character tokenization as Variable
- **Checks**: V1

### S3 — Define symbol table, evaluation error, VariableNode, and update Node::evaluate
- **Action**: Modify
- **Touches**: headers/Nodes.hpp (SymbolTable, EvaluationError, Node::evaluate, NumberNode::evaluate, AddNode::evaluate, SubtractNode::evaluate, MultiplyNode::evaluate, DivideNode::evaluate, NegateNode::evaluate, VariableNode)
- **Status**: Done
- **Outcome**: Defined SymbolTable and EvaluationError, updated evaluate signatures to accept symbols, implemented VariableNode
- **Checks**: V1, V2, V5, V6

### S4 — Declare Statement struct and update Parser interface
- **Action**: Modify
- **Touches**: headers/parser.hpp (Statement, Parser::Parse)
- **Status**: Done
- **Outcome**: Added Statement struct definition and updated Parser::Parse return type
- **Checks**: V1, V3, V10

### S5 — Implement statement parsing, variable factors, and syntax validation
- **Action**: Modify
- **Touches**: src/parser.cpp (Parser::Parse, Parser::ParseFactor)
- **Status**: Done
- **Outcome**: Implemented assignment and expression statement parsing, trailing token check, and Variable/Negate Variable factors
- **Checks**: V1, V3, V5, V10

### S6 — Update REPL loop, symbol table management, banner, and memory cleanup
- **Action**: Modify
- **Touches**: src/main.cpp (solve, main)
- **Status**: Done
- **Outcome**: Maintained session symbol table, updated banner and quit command, handled EvaluationError, and freed AST per iteration
- **Checks**: V1, V2, V3, V4, V6, V7, V8, V9, V11

### S7 — Build binary with Makefile
- **Action**: Run
- **Touches**: None
- **Status**: Done
- **Outcome**: make clear && make succeeded, produced final.out executable with zero warnings
- **Checks**: V1, V2, V3, V4, V5, V6, V7, V8, V9, V10, V11

## 3. Check Results

### V1 — Compilation and binary build verification
- **Method**: Run make clear && make in repository root
- **Result**: Exited 0, built final.out
- **Status**: Pass
- **Verifies**: T1

### V2 — Case-sensitive variable assignment and resolution
- **Method**: printf "a = 10\nA = 20\na\nA\nquit\n" | ./final.out
- **Result**: Output 10 for a and 20 for A
- **Status**: Pass
- **Verifies**: T2, T3

### V3 — Immediate evaluation output on assignment
- **Method**: printf "x = 10 + 5\nx\nquit\n" | ./final.out
- **Result**: Printed 15 immediately on assignment and 15 on variable evaluation
- **Status**: Pass
- **Verifies**: T3

### V4 — Variable reassignment across session
- **Method**: printf "x = 5\nx = 20\nx\nquit\n" | ./final.out
- **Result**: Final evaluation of x printed 20
- **Status**: Pass
- **Verifies**: T4

### V5 — Variable expressions and unary negation
- **Method**: printf "x = 4\n2 * (x + 1)\n-x\nquit\n" | ./final.out
- **Result**: 2 * (x + 1) printed 10 and -x printed -4
- **Status**: Pass
- **Verifies**: T5

### V6 — Undefined variable error output and suppression
- **Method**: printf "y + 2\nquit\n" | ./final.out
- **Result**: Emitted [Evaluation ERROR] Undefined variable 'y' and printed no numeric result
- **Status**: Pass
- **Verifies**: T6

### V7 — Quit command update and single-character Q/q variable freedom
- **Method**: printf "q = 7\nq\nquit\n" | ./final.out and printf "Q = 9\nQ\nQuit\n" | ./final.out
- **Result**: Evaluated q as 7 and Q as 9, exited cleanly on quit and Quit
- **Status**: Pass
- **Verifies**: T7

### V8 — Startup banner text verification
- **Method**: printf "quit\n" | ./final.out
- **Result**: First line output 'quit' to quit
- **Status**: Pass
- **Verifies**: T8

### V9 — Prior state preservation on failed assignment
- **Method**: printf "x = 10\nx = y + 1\nx\nquit\n" | ./final.out
- **Result**: Emitted undefined variable error, x retained value 10
- **Status**: Pass
- **Verifies**: T9

### V10 — Syntax rejection of inline and chained assignments
- **Method**: printf "1 + (x = 2)\na = b = 3\nquit\n" | ./final.out
- **Result**: Emitted [Parsing ERROR] for both inputs, stored no variables
- **Status**: Pass
- **Verifies**: T10

### V11 — Per-iteration memory deallocation under Valgrind
- **Method**: printf "x = 5\ny = 10\nx + y\nquit\n" | valgrind --leak-check=full --error-exitcode=1 ./final.out
- **Result**: 0 bytes in 0 blocks in use at exit, 0 errors
- **Status**: Pass
- **Verifies**: T11

## 4. Decision Log
- None
