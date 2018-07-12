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
void emit_mul(char* left) {
  printf("  imulq %s\n", left);
}
void emit_div(char* left) {
  printf("  movq $0, %%rdx\n");
  printf("  idivq %s\n", left);
}
void emit_return() {
  printf("  ret\n");
}

void emit_prologue(int localsize) {
  printf("  pushq %%rbp\n");
  printf("  movq %%rsp, %%rbp\n");
  printf("  subq $%d, %%rsp\n", localsize);
}
void emit_epilogue() {
  printf("  movq %%rbp, %%rsp\n");
  printf("  popq %%rbp\n");
}

void emit_localvarref(int pos) {
  printf("  movq -%d(%%rbp), %%rax\n", pos);
  printf("  pushq %%rax\n");
}
void emit_localvarset(int pos, char* value) {
  printf("  movq %s, -%d(%%rbp)\n", value, pos);
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
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_add("%rcx", "%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_SUB) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_sub("%rcx", "%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_MUL) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_mul("%rcx");
    emit_push("%rax");
  } else if (ast->kind == AST_DIV) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_div("%rcx");
    emit_push("%rax");
  } else if (ast->kind == AST_ASSIGN) {
    int pos = map_get(varmap, ast->left->ident);
    if (pos == -1) error("undeclared %s variable.", ast->left->ident);
    codegen(varmap, ast->right);
    emit_pop("%rax");
    emit_localvarset(pos, "%rax");
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}
