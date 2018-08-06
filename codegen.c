#include <stdio.h>
#include <assert.h>
#include "sncc.h"

#define emit_asm(...) {printf("  "); printf(__VA_ARGS__); printf("\n");}

int labelcnt = 0;

int gen_labeln() {
  labelcnt++;
  return labelcnt;
}

void emit_label(char* label) {
  printf("%s:\n", label);
}
void emit_labeln(int n) {
  printf(".L%d:\n", n);
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
} void emit_add(char* left, char* right) {
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
  } else if (ast->kind == AST_LESSER) {
    codegen(varmap, ast->left);
    codegen(varmap, ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("cmpq %%rcx, %%rax");
    emit_asm("setl %%al");
    emit_asm("movzbl %%al, %%eax");
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
    if (ast->arguments->len > 0) {
      codegen(varmap, vector_get(ast->arguments, 0));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rdi");
    }
    if (ast->arguments->len > 1) {
      codegen(varmap, vector_get(ast->arguments, 1));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rsi");
    }
    if (ast->arguments->len > 2) {
      codegen(varmap, vector_get(ast->arguments, 2));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rdx");
    }
    if (ast->arguments->len > 3) {
      codegen(varmap, vector_get(ast->arguments, 3));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rcx");
    }
    if (ast->arguments->len > 4) {
      codegen(varmap, vector_get(ast->arguments, 4));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%r8");
    }
    if (ast->arguments->len > 5) {
      codegen(varmap, vector_get(ast->arguments, 5));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%r9");
    }

    if (ast->arguments->len-6 >= 1) {
      emit_asm("subq $%d, %%rsp", (ast->arguments->len-6)*8);
    }
    for (int i=6; i<ast->arguments->len; i++) {
      codegen(varmap, vector_get(ast->arguments, i));
      emit_pop("%rax");
      emit_asm("movq %%rax, %d(%%rsp)", (i-6)*8);
    }
    emit_call(ast->call->ident);
    if (ast->arguments->len-6 >= 1) {
      emit_asm("addq $%d, %%rsp", (ast->arguments->len-6)*8);
    }
    emit_push("%rax");
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<statement_len(ast->stmt); i++) {
      astree* e = statement_get(ast->stmt, i);
      codegen(varmap, e);
    }
  } else if (ast->kind == AST_IF) {
    codegen(varmap, ast->ifcond);
    int elsel = gen_labeln();
    int endl = gen_labeln();
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je .L%d", elsel);
    codegen(varmap, ast->ifbody);
    emit_asm("jmp .L%d", endl);
    emit_labeln(elsel);
    if (ast->elsebody != NULL) {
      codegen(varmap, ast->elsebody);
    }
    emit_labeln(endl);
  } else if (ast->kind == AST_WHILE) {
    int startl = gen_labeln();
    int endl = gen_labeln();
    emit_labeln(startl);
    codegen(varmap, ast->whilecond);
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je .L%d", endl);
    codegen(varmap, ast->whilebody);
    emit_asm("jmp .L%d", startl);
    emit_labeln(endl);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void assign_variable_position(map* varmap, int* pos, astree* ast) {
  if (ast->kind == AST_ASSIGN) {
    if (ast->left->kind != AST_IDENT) error("current assign supports only variable.");
    *pos += 8;
    map_insert(varmap, ast->left->ident, *pos);
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<statement_len(ast->stmt); i++) {
      assign_variable_position(varmap, pos, statement_get(ast->stmt, i));
    }
  } else if (ast->kind == AST_IF) {
    assign_variable_position(varmap, pos, ast->ifbody);
  } else if (ast->kind == AST_WHILE) {
    assign_variable_position(varmap, pos, ast->whilebody);
  }
}

void codegen_funcdecl(funcdecl fdecl) {
  map* varmap = new_map();
  int varpos = 0;
  for (int i=0; i<paramtypelist_len(fdecl.argdecls); i++) {
    char* argname = paramtypelist_get(fdecl.argdecls, i);
    varpos += 8;
    map_insert(varmap, argname, varpos);
  }
  for (int i=0; i<statement_len(fdecl.body); i++) {
    assign_variable_position(varmap, &varpos, statement_get(fdecl.body, i));
  }
  emit_label(fdecl.fdecl);
  emit_prologue(varpos);
  int argpos = 0;
  for (int i=0; i<paramtypelist_len(fdecl.argdecls); i++) {
    char* argname = paramtypelist_get(fdecl.argdecls, i);
    int pos = map_get(varmap, argname);
    if (i == 0) {
      emit_localvarset(pos, "%rdi");
    } else if (i == 1) {
      emit_localvarset(pos, "%rsi");
    } else if (i == 2) {
      emit_localvarset(pos, "%rdx");
    } else if (i == 3) {
      emit_localvarset(pos, "%rcx");
    } else if (i == 4) {
      emit_localvarset(pos, "%r8");
    } else if (i == 5) {
      emit_localvarset(pos, "%r9");
    } else {
      argpos += 8;
      emit_asm("mov -%d(%%rbp), %%rax", argpos);
      emit_localvarset(pos, "%rax");
    }
  }
  for (int i=0; i<statement_len(fdecl.body); i++) {
    codegen(varmap, statement_get(fdecl.body, i));
  }
  emit_pop("%rax");
  emit_epilogue(varpos);
  emit_return();
}
