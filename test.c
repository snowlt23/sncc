
#define test(x, expect) test_impl(x, expect, #x, __LINE__)

int test_impl(int x, int expect, char* s, int line) {
  if (!(x == expect)) {
    printf("[ERROR] L%d %s: expect %d, but got %d", line, s, expect, x);
    exit(1);
  }
}

int infix_test() {
  test(1+2, 3);
  test(2-1, 1);
  test(1+10-2, 9);
  test(1+2*3, 7);
  test(7+2/2, 8);
  test((1+1)*2, 4);
  test((1+1)/2, 1);
  test(9 == 9, 1);
  test(9 == 8, 0);
  test(!0, 1);
  test(!1, 0);
  test(!10, 0);
  test(2 <= 5-2, 1);
  test(10 <= 5-2, 0);
}

int op_test() {
  int a = 0;
  test(++a, 1);
  test(a, 1);
  test(++a, 2);
  test(a, 2);

  int i = 0;
  test(i++, 0);
  test(i, 1);
  test(i++, 1);
  test(i, 2);
}

int variable_test() {
  int a = 9;
  int b = 8;
  test(a + b, 17);
}

int return_test_impl() {
  return 1+3*3;
}
int return_test() {
  test(return_test_impl(), 10);
}

int add(int a, int b) {
  return a + b;
}

int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
  return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
}

int call_test() {
  test(add(4, 5), 9);
  test(add8(1, 1, 1, 1, 1, 1, 1, 1), 8);
}

int if_test_impl1() {
  if (1) return 9;
  if (0) return 5;
}

int if_test_impl2() {
  if (0) return 9;
  if (1) return 5;
}

int if_test_impl3() {
  if (0) {
    return 9;
  }
  if (1) {
    return 5;
  }
}

int if_test() {
  test(if_test_impl1(), 9);
  test(if_test_impl2(), 5);
  test(if_test_impl3(), 5);
}

int for_test_impl() {
  for (int i=0; i<10; i++) {
    if (i == 5) return i;
  }
}

int for_test() {
  for (int i=0; i<10; i++) {
  }
  test(i, 10);
  test(for_test_impl(), 5);
}

int addr_test() {
  int x;
  int* p = &x;
  *p = 9;
  test(x, 9);

  int** pp = &p;
  **pp = 555;
  test(x, 555);
}

int pointer_arithmetic_test() {
  int arr[10];
  int* p = arr;
  *(p+0) = 1;
  *(p+1) = 2;
  *(p+2) = 3;
  *(p+3) = 4;
  test(arr[0] + arr[1] + arr[2] + arr[3], 10);
}

int fib(int n) {
  if (n < 2) {
    return n;
  } else {
    return fib(n-1) + fib(n-2);
  }
}

int fib_test() {
  test(fib(10), 55);
}

int sizeof_test() {
  test(sizeof(int), 4);
  test(sizeof(int*), 8);
  test(sizeof int, 4);
  test(sizeof int*, 8);
  int iarr[10];
  int* parr[10];
  test(sizeof iarr, 40);
  test(sizeof parr, 80);
}



int array_test() {
  int arr[10];
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  test(arr[0] + arr[1] + arr[2] + arr[3], 10);

  *&arr[0] = 10;
  test(arr[0], 10);
}

int gx;
int garr[10];
int globalvar_test() {
  gx = 9;
  test(gx, 9);
  int* p = &gx;
  *p = 555;
  test(gx, 555);

  garr[0] = 1;
  garr[1] = 9;
  test(garr[0] + garr[1], 10);
}

int string_test() {
  char cc[3];
  cc[0] = 9;
  test(cc[0], 9);

  char* s = "yukaia";
  test(s[0], 121);
}

int N;
int S;
int dfs(int i, int sum, int cnt) {
  if (cnt == N) {
    if (sum == S) {
      return 1;
    }
  }
  if (i == 10) return 0;
  if (cnt == N) return 0;
  
  int acc = 0;
  acc = acc+dfs(i+1, sum+i, cnt+1);
  acc = acc+dfs(i+1, sum, cnt);
  return acc;
}

int aoj0030() {
  N = 3;
  S = 6;
  test(dfs(0, 0, 0), 3);
}

int GI = 9;
char* GS = "yukaia";
int globalinit_test() {
  test(GI, 9);
  test(GS[0], 121);
}

int* malloc(int size);

int malloc_test() {
  int* p = malloc(sizeof(int)*4);
  p[0] = 1;
  p[1] = 2;
  p[2] = 3;
  p[3] = 4;
  test(p[0] + p[1] + p[2] + p[3], 10);
}

int main() {
  infix_test();
  op_test();
  variable_test();
  return_test();
  call_test();
  if_test();
  addr_test();
  pointer_arithmetic_test();
  fib_test();
  sizeof_test();
  array_test();
  globalvar_test();
  string_test();
  aoj0030();
  globalinit_test();
  malloc_test();

  printf("[OK]");
  return 0;
}
