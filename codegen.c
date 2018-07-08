
#include <stdio.h>

void emit_global(char* name) {
  printf("  .global %s\n", name);
}
void emit_label(char* label) {
  printf("%s:\n", label);
}

void emit_push_int(int x) {
  printf("  pushq $%d\n", x);
}
void emit_push(char* s) {
  printf("  pushq %s\n", s);
}
void emit_pop(char* s) {
  printf("  popq %s\n", s);
}
void emit_add(char* left, char* right) {
  printf("  addq %s, %s\n", left, right);
}
void emit_sub(char* left, char* right) {
  printf("  subq %s, %s\n", left, right);
}
void emit_return() {
  printf("  ret\n");
}

void codegen(astree* ast) {
  if (ast->kind == AST_INTLIT) {
    emit_push_int(ast->intval);
  } else if (ast->kind == AST_ADD) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_add("%rcx", "%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_SUB) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_sub("%rcx", "%rax");
    emit_push("%rax");
  } else {
    assert(0);
  }
}
