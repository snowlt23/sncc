
#include <stdio.h>
#include <assert.h>
#include "sncc.h"

void emit_global(char* name) {
  printf("  .global %s\n", name);
}
void emit_label(char* label) {
  printf("%s:\n", label);
}

void emit_push_int(int x) {
  printf("  pushl $%d\n", x);
}
void emit_push(char* s) {
  printf("  pushl %s\n", s);
}
void emit_pop(char* s) {
  printf("  popl %s\n", s);
}
void emit_add(char* left, char* right) {
  printf("  addl %s, %s\n", left, right);
}
void emit_sub(char* left, char* right) {
  printf("  subl %s, %s\n", left, right);
}
void emit_mul(char* left) {
  printf("  imull %s\n", left);
}
void emit_div(char* left) {
  printf("  movl $0, %%edx\n");
  printf("  idivl %s\n", left);
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
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_add("%ecx", "%eax");
    emit_push("%eax");
  } else if (ast->kind == AST_SUB) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_sub("%ecx", "%eax");
    emit_push("%eax");
  } else if (ast->kind == AST_MUL) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_mul("%ecx");
    emit_push("%eax");
  } else if (ast->kind == AST_DIV) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_div("%ecx");
    emit_push("%eax");
  } else {
    assert(false);
  }
}
