unittest() {
  gcc -otest.out -Wall $1
  ./test.out
  RET=$?
  if [ ! $RET = 0 ]; then
    echo "[ERROR] unittest: $1"
    exit 1
  fi
}

lexertest() {
  OUT=`echo "$1" | ./lexer.out`
  OUT=`echo $OUT`
  if [ ! "$OUT" = "$2" ]; then
    echo "[ERROR] lexer: $1, expected $2, but got $OUT"
    exit 1
  fi
}

exprtest() {
  OUT=`echo "$1" | ./parser_expr.out`
  OUT=`echo $OUT`
  if [ ! "$OUT" = "$2" ]; then
    echo "[ERROR] expr: $1, expected $2, but got $OUT"
    exit 1
  fi
}

stmttest() {
  OUT=`echo "$1" | ./parser_stmt.out`
  OUT=`echo $OUT`
  if [ ! "$OUT" = "$2" ]; then
    echo "[ERROR] stmt: $1, expected $2, but got $OUT"
    exit 1
  fi
}

funcdecltest() {
  OUT=`echo "$1" | ./funcdecl.out`
  OUT=`echo $OUT`
  if [ ! "$OUT" = "$2" ]; then
    echo "[ERROR] funcdecl: $1, expected $2, but got $OUT"
    exit 1
  fi
}

unittest "test/vector_test.c"
unittest "test/map_test.c"

lexertest "9" "TOKEN_INTLIT:9"
lexertest "12345" "TOKEN_INTLIT:12345"
lexertest "\"yukaia\"" "TOKEN_STRLIT:yukaia"
lexertest "yukari" "TOKEN_IDENT:yukari"
lexertest "+" "TOKEN_ADD:+"
lexertest "-" "TOKEN_SUB:-"
lexertest "*" "TOKEN_MUL:*"
lexertest "/" "TOKEN_DIV:/"
lexertest "=" "TOKEN_ASSIGN:="
lexertest "&" "TOKEN_AND:&"
lexertest "(" "TOKEN_LPAREN:("
lexertest ")" "TOKEN_RPAREN:)"
lexertest "[" "TOKEN_LBRACKET:["
lexertest "]" "TOKEN_RBRACKET:]"
lexertest "{" "TOKEN_LBRACE:{"
lexertest "}" "TOKEN_RBRACE:}"
lexertest "," "TOKEN_COMMA:,"
lexertest ";" "TOKEN_SEMICOLON:;"
lexertest "1 + 2" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2"
lexertest "3 - 4" "TOKEN_INTLIT:3 TOKEN_SUB:- TOKEN_INTLIT:4"
lexertest "1 + 2 - 3" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_INTLIT:2 TOKEN_SUB:- TOKEN_INTLIT:3"
lexertest "1 + (2 + 3)" "TOKEN_INTLIT:1 TOKEN_ADD:+ TOKEN_LPAREN:( TOKEN_INTLIT:2 TOKEN_ADD:+ TOKEN_INTLIT:3 TOKEN_RPAREN:)"
lexertest "yukari + ia" "TOKEN_IDENT:yukari TOKEN_ADD:+ TOKEN_IDENT:ia"
lexertest "a = 1" "TOKEN_IDENT:a TOKEN_ASSIGN:= TOKEN_INTLIT:1"
lexertest "n < 2" "TOKEN_IDENT:n TOKEN_LESSER:< TOKEN_INTLIT:2"
lexertest "n > 2" "TOKEN_IDENT:n TOKEN_GREATER:> TOKEN_INTLIT:2"
lexertest "i++" "TOKEN_IDENT:i TOKEN_INC:++"

exprtest "1" "AST_INTLIT"
exprtest "1 + 2" "AST_ADD"
exprtest "1 - 2" "AST_SUB"
exprtest "2 * 1" "AST_MUL"
exprtest "2 / 1" "AST_DIV"
exprtest "2 < 1" "AST_LESSER"
exprtest "2 > 1" "AST_LESSER"
exprtest "2 * 1 + 3" "AST_ADD"
exprtest "2 / 1 + 3" "AST_ADD"
exprtest "1 + 2 * 3" "AST_ADD"
exprtest "1 + 2 / 3" "AST_ADD"
exprtest "(1 + 1) * 2" "AST_MUL"
exprtest "(1 + 1) / 2" "AST_DIV"
exprtest "2 * (1 + 1)" "AST_MUL"
exprtest "2 / (1 + 1)" "AST_DIV"
exprtest "-9" "AST_MINUS"
exprtest "a = 1" "AST_ASSIGN"
exprtest "a = 1 + 2 * 3" "AST_ASSIGN"
exprtest "fib()" "AST_CALL"
exprtest "&a" "AST_ADDR"
exprtest "*a" "AST_DEREF"
exprtest "\"yukaia\"" "AST_STRLIT"
exprtest "*a = 1" "AST_ASSIGN"
exprtest "i++" "AST_POSTINC"
exprtest "i+1++" "AST_ADD"
exprtest "++i" "AST_PREINC"
exprtest "++i++" "AST_PREINC"
exprtest "sizeof(int)*4" "AST_MUL"
exprtest "struc.a" "AST_DOT"
exprtest "struc.a + struc.b" "AST_ADD"

stmttest "1; 2; 3;" "3 AST_INTLIT AST_INTLIT AST_INTLIT"
stmttest "1 + 1; 2 - 2; 3 * 3;" "3 AST_ADD AST_SUB AST_MUL"
stmttest "a = 1; a;" "2 AST_ASSIGN AST_IDENT"
stmttest "if (1) 9;" "1 AST_IF"
stmttest "if (1) {9;}" "1 AST_IF"
stmttest "if (1) {9;} else {12;}" "1 AST_IF"
stmttest "while (1) {1;}" "1 AST_WHILE"
stmttest "int a;" "1 AST_VARDECL"
stmttest "return 1;" "1 AST_RETURN"
stmttest "int a = 1;" "1 AST_VARDECL"

funcdecltest "int* malloc(int size);" "malloc size"
funcdecltest "int main() {}" "main"
funcdecltest "int fib(int n) {n;}" "fib n"
funcdecltest "int padd(int* a, int* b) {int* c; c = a + b;}" "padd a b"
funcdecltest "struct myint { int x; int y; };" "myint x y"
funcdecltest "struct myptr { int* p; };" "myptr p"

echo "[OK]"
