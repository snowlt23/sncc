
rettest() {
  echo "$2" | ./sncc > test.s
  gcc -otest.out -m32 test.s
  ./test.out
  RETCODE=$?
  if [ $RETCODE = $3 ]; then
    echo "[OK] $1"
  else
    echo "[ERROR] $1, expected $3, but got $RETCODE"
  fi
}

rettest "retcode: 2" "2;" 2
rettest "retcode: 9" "9;" 9
rettest "retcode: 1 + 2" "1 + 2;" 3
rettest "retcode: 2 - 1" "2 - 1;" 1
rettest "retcode: 1 + 10 - 2" "1 + 10 - 2;" 9
rettest "retcode: 1 + 2 * 3" "1 + 2 * 3;" 7
rettest "retcode: 7 + 2 / 2" "7 + 2 / 2;" 8
rettest "retcode: (1 + 1) * 2" "(1 + 1) * 2;" 4
rettest "retcode: (1 + 1) / 2" "(1 + 1) / 2;" 1
rettest "retcode-stmt: 1; 2; 5;" "1; 2; 5;" 5
rettest "retcode-stmt: 1; 2; 3*3;" "1; 2; 3*3;" 9
rettest "retcode-stmt: 1; 2; 1+3*3;" "1; 2; 1+3*3;" 10
