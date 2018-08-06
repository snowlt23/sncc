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

rettest "main() {1;}" 1
rettest "main() {9;}" 9
rettest "main() {1 + 2;}" 3
rettest "main() {2 - 1;}" 1
rettest "main() {1 + 10 - 2;}" 9
rettest "main() {1 + 2 * 3;}" 7
rettest "main() {7 + 2 / 2;}" 8
rettest "main() {(1 + 1) * 2;}" 4
rettest "main() {(1 + 1) / 2;}" 1
rettest "main() {1; 2; 5;}" 5
rettest "main() {1; 2; 3*3;}" 9
rettest "main() {1; 2; 1+3*3;}" 10
rettest "main() {-5;}" 251
rettest "main() {5 + (-5);}" 0
rettest "main() {9 + -5;}" 4
rettest "main() {a = 9; a;}" 9
rettest "main() {a = 3; b = 4; a + b;}" 7
rettest "main() {th=3; fo=4; th*fo;}" 12
rettest "main() {num9();}" 9
rettest "main() {num9() + num9();}" 18
rettest "main() {add5(4);}" 9
rettest "main() {div(10, 2);}" 5
rettest "main() {multidiv6(100, 1, 2, 1, 2, 5);}" 5
rettest "main() {multidiv6_2(200, 1, 2, 1, 2, 1, 10, 5);}" 25
rettest "add(a, b) {a + b;} main() {add(4, 5);}" 9
rettest "add8(a1, a2, a3, a4, a5, a6, a7, a8) {a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;} main() {add8(1, 1, 1, 1, 1, 1, 1, 1);}" 8
rettest "main() { if (1) 9; if (0) 5; }" 9
rettest "main() { if (0) {9;} if (1) {5;} }" 5
rettest "main() { if (0) 39; else 14; }" 14
rettest "fib(n) { if (n < 2) {n;} else {fib(n-1) + fib(n-2);} } main() { fib(10); }" 55
rettest "f(x) { if (x > 5) 5; else 10; } main() { f(100); }" 5
rettest "main() { i=0; while (i<5) { i = i+1; } i; }" 5
