
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
  test(0, 1);
}

int main() {
  infix_test();
}
