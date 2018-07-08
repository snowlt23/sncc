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

lexertest () {
  gcc -otest.out test/lexer_test.c
  OUT=`echo "$2" | ./test.out`
  OUT=`echo $OUT`
  if [ "$OUT" = "$3" ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $OUT"
  fi
}

unittest "vector" "test/vector_test.c" "12345"

lexertest "lexer: 9" "9" "TOKEN_INTLIT:9"
lexertest "lexer: +" "+" "TOKEN_ADD:+"
lexertest "lexer: -" "-" "TOKEN_SUB:-"
lexertest "lexer: 1 + 2" "1 + 2" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2"
lexertest "lexer: 3 - 4" "3 - 4" "TOKEN_INTLIT:3 TOKEN_SUB:- TOKEN_INTLIT:4"
lexertest "lexer: 1 + 2 - 3" "1 + 2 - 3" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2 TOKEN_SUB:- TOKEN_INTLIT:3"
