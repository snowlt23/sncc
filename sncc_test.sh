gcc -c sncclib.c

rettest() {
  echo "$1" | ./sncc > test.s
  gcc -otest.out test.s sncclib.o
  ./test.out
  RETCODE=$?
  if [ $RETCODE = $2 ]; then
    echo "[OK] rettest: $1"
  else
    echo "[ERROR] rettest: $1, expected $2, but got $RETCODE"
    exit 1
  fi
}

rettest "1;" 1
rettest "9;" 9
rettest "1 + 2;" 3
rettest "2 - 1;" 1
rettest "1 + 10 - 2;" 9
rettest "1 + 2 * 3;" 7
rettest "7 + 2 / 2;" 8
rettest "(1 + 1) * 2;" 4
rettest "(1 + 1) / 2;" 1
rettest "1; 2; 5;" 5
rettest "1; 2; 3*3;" 9
rettest "1; 2; 1+3*3;" 10
rettest "-5;" 251
rettest "5 + (-5);" 0
rettest "9 + -5;" 4
rettest "a = 9; a;" 9
rettest "a = 3; b = 4; a + b;" 7
rettest "th=3; fo=4; th*fo;" 12
rettest "num9();" 9
rettest "num9() + num9();" 18
rettest "add5(4);" 9
rettest "div(10, 2);" 5
rettest "multidiv6(100, 1, 2, 1, 2, 5);" 5
rettest "multidiv6_2(200, 1, 2, 1, 2, 1, 10, 5);" 25
