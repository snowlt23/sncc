int fib(int n) {
  if (n < 2) {
    n;
  } else {
    fib(n-1) + fib(n-2);
  }
}

int main() {
  printf("%d", fib(10));
}
