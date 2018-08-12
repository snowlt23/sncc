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

void codegen_lvalue(astree* ast) {
  if (ast->kind == AST_IDENT) {
    emit_asm("leaq -%d(%%rbp), %%rax", ast->offset);
    emit_push("%rax");
  } else if (ast->kind == AST_DEREF) {
    codegen(ast->value);
  } else {
    error("%s isn't lvalue.", ast_to_kindstr(ast));
  }
}

void codegen(astree* ast) {
  if (ast->kind == AST_IDENT) {
    if (ast->typ->truetype != NULL && ast->typ->truetype->kind == TYPE_ARRAY) {
      codegen_lvalue(ast);
    } else {
      codegen_lvalue(ast);
      emit_pop("%rax");
      emit_asm("movq (%%rax), %%rax");
      emit_push("%rax");
    }
  } else if (ast->kind == AST_INTLIT) {
    emit_push_int(ast->intval);
  } else if (ast->kind == AST_ADD) {
    codegen(ast->left);
    codegen(ast->right);
    if (ast->left->typ->kind == TYPE_PTR) { // pointer arithmetic
      emit_pop("%rax");
      emit_asm("movq $%d, %%rcx", typesize(ast->left->typ->ptrof));
      emit_asm("imulq %%rcx");
      emit_push("%rax");
    }
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_add("%rcx", "%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_SUB) {
    codegen(ast->left);
    codegen(ast->right);
    assert(ast->left->typ != NULL);
    if (ast->left->typ->kind == TYPE_PTR) { // pointer arithmetic
      emit_pop("%rax");
      emit_asm("imulq $%d", typesize(ast->left->typ->ptrof));
      emit_push("%rax");
    }
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_sub("%rcx", "%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_MUL) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_mul("%rcx");
    emit_push("%rax");
  } else if (ast->kind == AST_DIV) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_div("%rcx");
    emit_push("%rax");
  } else if (ast->kind == AST_LESSER) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("cmpq %%rcx, %%rax");
    emit_asm("setl %%al");
    emit_asm("movzbl %%al, %%eax");
    emit_push("%rax");
  } else if (ast->kind == AST_MINUS) {
    codegen(ast->value);
    emit_pop("%rax");
    emit_asm("negq %%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_ASSIGN) {
    codegen_lvalue(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("movq %%rcx, (%%rax)");
  } else if (ast->kind == AST_ADDR) {
    codegen_lvalue(ast->value);
  } else if (ast->kind == AST_DEREF) {
    codegen(ast->value);
    emit_pop("%rax");
    emit_asm("pushq (%%rax)");
  } else if (ast->kind == AST_VARDECL) {
    // discard
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    if (ast->arguments->len > 0) {
      codegen(vector_get(ast->arguments, 0));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rdi");
    }
    if (ast->arguments->len > 1) {
      codegen(vector_get(ast->arguments, 1));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rsi");
    }
    if (ast->arguments->len > 2) {
      codegen(vector_get(ast->arguments, 2));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rdx");
    }
    if (ast->arguments->len > 3) {
      codegen(vector_get(ast->arguments, 3));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%rcx");
    }
    if (ast->arguments->len > 4) {
      codegen(vector_get(ast->arguments, 4));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%r8");
    }
    if (ast->arguments->len > 5) {
      codegen(vector_get(ast->arguments, 5));
      emit_pop("%rax");
      emit_asm("movq %%rax, %%r9");
    }

    if (ast->arguments->len-6 >= 1) {
      emit_asm("subq $%d, %%rsp", (ast->arguments->len-6)*8);
    }
    for (int i=6; i<ast->arguments->len; i++) {
      codegen(vector_get(ast->arguments, i));
      emit_pop("%rax");
      emit_asm("movq %%rax, %d(%%rsp)", (i-6)*8);
    }
    emit_call(ast->call->ident);
    if (ast->arguments->len-6 >= 1) {
      emit_asm("addq $%d, %%rsp", (ast->arguments->len-6)*8);
    }
    emit_push("%rax");
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<ast->stmt->len; i++) {
      astree* e = vector_get(ast->stmt, i);
      codegen(e);
    }
  } else if (ast->kind == AST_IF) {
    codegen(ast->ifcond);
    int elsel = gen_labeln();
    int endl = gen_labeln();
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je .L%d", elsel);
    codegen(ast->ifbody);
    emit_asm("jmp .L%d", endl);
    emit_labeln(elsel);
    if (ast->elsebody != NULL) {
      codegen(ast->elsebody);
    }
    emit_labeln(endl);
  } else if (ast->kind == AST_WHILE) {
    int startl = gen_labeln();
    int endl = gen_labeln();
    emit_labeln(startl);
    codegen(ast->whilecond);
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je .L%d", endl);
    codegen(ast->whilebody);
    emit_asm("jmp .L%d", startl);
    emit_labeln(endl);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void codegen_funcdecl(funcdecl fdecl) {
  emit_label(fdecl.fdecl->name);
  emit_prologue(fdecl.stacksize);
  int argpos = 0;
  for (int i=0; i<fdecl.argdecls->len; i++) {
    paramtype* argparam = vector_get(fdecl.argdecls, i);
    int offset = argparam->offset;
    if (i == 0) {
      emit_localvarset(offset, "%rdi");
    } else if (i == 1) {
      emit_localvarset(offset, "%rsi");
    } else if (i == 2) {
      emit_localvarset(offset, "%rdx");
    } else if (i == 3) {
      emit_localvarset(offset, "%rcx");
    } else if (i == 4) {
      emit_localvarset(offset, "%r8");
    } else if (i == 5) {
      emit_localvarset(offset, "%r9");
    } else {
      argpos += 8;
      emit_asm("mov -%d(%%rbp), %%rax", argpos);
      emit_localvarset(offset, "%rax");
    }
  }
  for (int i=0; i<fdecl.body->len; i++) {
    codegen(vector_get(fdecl.body, i));
  }
  emit_pop("%rax");
  emit_epilogue(fdecl.stacksize);
  emit_return();
}
