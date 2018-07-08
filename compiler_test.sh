
unittest() {
  gcc -otest.out $2
  OUT=`./test.out`
  if [ $OUT = $3 ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $OUT"
  fi
}

unittest "vector" "test/vector_test.c" "12345"
