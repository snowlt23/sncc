#include <stdio.h>
#include <assert.h>
#include "sncc.h"

#define emit_asm(...) {printf("  "); printf(__VA_ARGS__); printf("\n");}

void emit_label(char* label) {
  printf("%s:\n", label);
}
void emit_global(char* name) {
  emit_asm(".global %s", name);
}

void emit_push_int(int x) {
  emit_asm("pushq $%d", x);
}
void emit_push(char* s) {
  emit_asm("pushq %s", s);
}
void emit_pop(char* s) {
  emit_asm("popq %s", s);
}
void emit_add(char* left, char* right) {
  emit_asm("addq %s, %s", left, right);
}
void emit_sub(char* left, char* right) {
  emit_asm("subq %s, %s", left, right);
}
void emit_mul(char* left) {
  emit_asm("imulq %s", left);
}
void emit_div(char* left) {
  emit_asm("movq $0, %%rdx");
  emit_asm("idivq %s", left);
}
void emit_return() {
  emit_asm("ret");
}
void emit_call(char* name) {
  emit_asm("call %s", name);
}

void emit_prologue(int localsize) {
  emit_asm("pushq %%rbp");
  emit_asm("movq %%rsp, %%rbp");
  emit_asm("subq $%d, %%rsp", localsize);
}
void emit_epilogue() {
  emit_asm("movq %%rbp, %%rsp");
  emit_asm("pop %%rbp");
}

void emit_localvarref(int pos) {
  emit_asm("movq -%d(%%rbp), %%rax", pos);
  emit_asm("pushq %%rax");
}
void emit_localvarset(int pos, char* value) {
  emit_asm("movq %s, -%d(%%rbp)", value, pos);
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
  } else if (ast->kind == AST_MINUS) {
    codegen(varmap, ast->value);
    emit_pop("%rax");
    emit_asm("negq %%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_ASSIGN) {
    int pos = map_get(varmap, ast->left->ident);
    if (pos == -1) error("undeclared %s variable.", ast->left->ident);
    codegen(varmap, ast->right);
    emit_pop("%rax");
    emit_localvarset(pos, "%rax");
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    for (int i=0; i<ast->arguments->len; i++) {
      astree* arg = vector_get(ast->arguments, i);
      codegen(varmap, arg);
      emit_pop("%rax");
      if (i == 0) {
        emit_asm("movq %%rax, %%rdi");
      } else if (i == 1) {
        emit_asm("movq %%rax, %%rsi");
      } else if (i == 2) {
        emit_asm("movq %%rax, %%rdx");
      } else if (i == 3) {
        emit_asm("movq %%rax, %%rcx");
      } else if (i == 4) {
        emit_asm("movq %%rax, %%r8");
      } else if (i == 5) {
        emit_asm("movq %%rax, %%r9");
      } else {
        emit_push("%%rax");
      }
    }
    emit_call(ast->call->ident);
    emit_push("%rax");
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}
