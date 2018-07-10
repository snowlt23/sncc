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

void emit_prologue(int localsize) {
  printf("  pushl %%ebp\n");
  printf("  movl %%esp, %%ebp\n");
  printf("  subl $%d, %%esp\n", localsize);
}
void emit_epilogue() {
  printf("  movl %%ebp, %%esp\n");
  printf("  popl %%ebp\n");
}

void emit_localvarref(int pos) {
  printf("  movl -%d(%%ebp), %%eax\n", pos);
  printf("  pushl %%eax\n");
}
void emit_localvarset(int pos, char* value) {
  printf("  movl %s, -%d(%%ebp)\n", value, pos);
}

void codegen(map* varmap, astree* ast) {
  if (ast->kind == AST_IDENT) {
    int pos = map_get(varmap, ast->ident);
    if (pos == -1) error("undeclared %s variable.", ast->ident);
    emit_localvarref(pos);
  } else if (ast->kind == AST_INTLIT) {
    emit_push_int(ast->intval);
  } else if (ast->kind == AST_ADD) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_add("%ecx", "%eax");
    emit_push("%eax");
  } else if (ast->kind == AST_SUB) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_sub("%ecx", "%eax");
    emit_push("%eax");
  } else if (ast->kind == AST_MUL) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_mul("%ecx");
    emit_push("%eax");
  } else if (ast->kind == AST_DIV) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%ecx");
    emit_pop("%eax");
    emit_div("%ecx");
    emit_push("%eax");
  } else if (ast->kind == AST_ASSIGN) {
    int pos = map_get(varmap, ast->left->ident);
    if (pos == -1) error("undeclared %s variable.", ast->left->ident);
    codegen(varmap, ast->right);
    emit_pop("%eax");
    emit_localvarset(pos, "%eax");
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}
