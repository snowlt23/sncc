unittest() {
  gcc -otest.out $2
  OUT=`./test.out`
  OUT=`echo $OUT`
  if [ "$OUT" = "$3" ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $OUT"
  fi
}

lexertest() {
  OUT=`echo "$2" | ./lexer.out`
  OUT=`echo $OUT`
  if [ "$OUT" = "$3" ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $OUT"
  fi
}

parsertest() {
  OUT=`echo "$2" | ./parser.out`
  OUT=`echo $OUT`
  if [ "$OUT" = "$3" ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $OUT"
  fi
}

unittest "vector" "test/vector_test.c" "12345"

gcc -olexer.out test/lexer_test.c
gcc -oparser.out test/parser_test.c

lexertest "lexer: 9" "9" "TOKEN_INTLIT:9"
lexertest "lexer: 12345" "12345" "TOKEN_INTLIT:12345"
lexertest "lexer: +" "+" "TOKEN_ADD:+"
lexertest "lexer: -" "-" "TOKEN_SUB:-"
lexertest "lexer: *" "*" "TOKEN_MUL:*"
lexertest "lexer: /" "/" "TOKEN_DIV:/"
lexertest "lexer: (" "(" "TOKEN_LPAREN:("
lexertest "lexer: )" ")" "TOKEN_RPAREN:)"
lexertest "lexer: 1 + 2" "1 + 2" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2"
lexertest "lexer: 3 - 4" "3 - 4" "TOKEN_INTLIT:3 TOKEN_SUB:- TOKEN_INTLIT:4"
lexertest "lexer: 1 + 2 - 3" "1 + 2 - 3" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2 TOKEN_SUB:- TOKEN_INTLIT:3"

parsertest "parser: 1" "1" "AST_INTLIT"
parsertest "parser: 1 + 2" "1 + 2" "AST_ADD"
parsertest "parser: 1 - 2" "1 - 2" "AST_SUB"
parsertest "parser: 2 * 1" "2 * 1" "AST_MUL"
parsertest "parser: 2 / 1" "2 / 1" "AST_DIV"
parsertest "parser: 2 * 1 + 3" "2 * 1 + 3" "AST_ADD"
parsertest "parser: 2 / 1 + 3" "2 / 1 + 3" "AST_ADD"
