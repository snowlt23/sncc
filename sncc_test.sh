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

rettest "int main() {1;}" 1
rettest "int main() {9;}" 9
rettest "int main() {1 + 2;}" 3
rettest "int main() {2 - 1;}" 1
rettest "int main() {1 + 10 - 2;}" 9
rettest "int main() {1 + 2 * 3;}" 7
rettest "int main() {7 + 2 / 2;}" 8
rettest "int main() {(1 + 1) * 2;}" 4
rettest "int main() {(1 + 1) / 2;}" 1
rettest "int main() {1; 2; 5;}" 5
rettest "int main() {1; 2; 3*3;}" 9
rettest "int main() {1; 2; 1+3*3;}" 10
rettest "int main() {-5;}" 251
rettest "int main() {5 + (-5);}" 0
rettest "int main() {9 + -5;}" 4
rettest "int main() {int a; a = 9; a;}" 9
rettest "int main() {int a; int b; a = 3; b = 4; a + b;}" 7
rettest "int main() {int th; int fo; th=3; fo=4; th*fo;}" 12
rettest "int main() {num9();}" 9
rettest "int main() {num9() + num9();}" 18
rettest "int main() {add5(4);}" 9
rettest "int main() {adiv(10, 2);}" 5
rettest "int main() {multidiv6(100, 1, 2, 1, 2, 5);}" 5
rettest "int main() {multidiv6_2(200, 1, 2, 1, 2, 1, 10, 5);}" 25
rettest "int add(int a, int b) {a + b;} int main() {add(4, 5);}" 9
rettest "int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;} int main() {add8(1, 1, 1, 1, 1, 1, 1, 1);}" 8
rettest "int main() { if (1) 9; if (0) 5; }" 9
rettest "int main() { if (0) {9;} if (1) {5;} }" 5
rettest "int main() { if (0) 39; else 14; }" 14
rettest "int fib(int n) { if (n < 2) {n;} else {fib(n-1) + fib(n-2);} } int main() { fib(10); }" 55
rettest "int f(int x) { if (x > 5) 5; else 10; } int main() { f(100); }" 5
rettest "int main() { int i; i=0; while (i<5) { i = i+1; } i; }" 5
rettest "int main() { int i; for (i=0; i<10; i=i+1) {i = i+1;} i; }" 10
rettest "int main() { int x; x = 3; int* y; y = &x; *y; }" 3
rettest "int main() { int x; x = 3; int* y; y = &x; *y = 9; x; }" 9
rettest "int main() { int x; x = 3; int* y; y = &x; int** z; z = &y; **z = 9; x; }" 9
rettest "int main() { int* p; alloc4(&p, 1, 2, 4, 8); int* q; q = p + 2; int a; a = *q; q = p + 3; int b; b = *q; a + b; }" 12
rettest "int main() { sizeof(int); }" 4
rettest "int main() { sizeof(int*); }" 8
rettest "int main() { int a; sizeof(a); }" 4
rettest "int main() { int* a; sizeof(a); }" 8
rettest "int main() { int a[10]; sizeof(a); }" 40
rettest "int main() { int* a[10]; sizeof(a); }" 80
rettest "int main() { int a[4]; a[0] = 1; a[1] = 2; a[2] = 3; a[3] = 4; a[0]+a[1]+a[2]+a[3]; }" 10
rettest "int* ptrid(int* p) {p;} int main() { int a; int* b; b = ptrid(&a); *b = 5; a; }" 5
