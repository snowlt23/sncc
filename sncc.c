
#include <stdio.h>

void emit_global(char* name) {
  printf("  .global %s\n", name);
}
void emit_label(char* label) {
  printf("%s:\n", label);
}
void emit_returncode(int ret) {
  printf("  mov $%d, %%eax\n", ret);
  printf("  ret\n");
}

int main() {
  int returncode;
  scanf("%d", &returncode);
  emit_global("main");
  emit_label("main");
  emit_returncode(returncode);
  return 0;
}
