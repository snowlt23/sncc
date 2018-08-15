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
  } else if (ast->kind == AST_GLOBALREF) {
    emit_asm("leaq %s(%%rip), %%rax", ast->ident);
    emit_push("%rax");
  } else if (ast->kind == AST_DEREF) {
    codegen(ast->value);
  } else if (ast->kind == AST_DOT) {
    paramtype* field = get_field(ast->structvalue->typ, ast->fieldname);
    codegen_lvalue(ast->structvalue);
    emit_pop("%rax");
    emit_asm("addq $%d, %%rax", field->offset);
    emit_push("%rax");
  } else {
    error("%s isn't lvalue.", ast_to_kindstr(ast));
  }
}

void codegen_movevalue(typenode* typ) {
  if (typ->truetype != NULL && typ->truetype->kind == TYPE_ARRAY) {
  } else if (typ->kind == TYPE_CHAR) {
    emit_pop("%rax");
    emit_asm("movsbl (%%rax), %%eax");
    emit_push("%rax");
  } else if (typ->kind == TYPE_INT) {
    emit_pop("%rax");
    emit_asm("movl (%%rax), %%eax");
    emit_push("%rax");
  } else if (typ->kind == TYPE_PTR) {
    emit_pop("%rax");
    emit_asm("movq (%%rax), %%rax");
    emit_push("%rax");
  } else {
    assert(false);
  }
}

void codegen_movereg(typenode* typ, char* reg8, char* reg32, char* reg64) {
  if (typ->kind == TYPE_CHAR) {
    emit_asm("movb %s, (%%rax)", reg8);
  } else if (typ->kind == TYPE_INT) {
    emit_asm("movl %s, (%%rax)", reg32);
  } else if (typ->kind == TYPE_PTR) {
    emit_asm("movq %s, (%%rax)", reg64);
  } else {
    assert(false);
  }
}

void codegen_add(typenode* typ) {
  if (typ->kind == TYPE_PTR) { // pointer arithmetic
    emit_pop("%rax");
    emit_asm("movq $%d, %%rcx", typesize(typ->ptrof));
    emit_asm("imulq %%rcx");
    emit_push("%rax");
  }
  emit_pop("%rcx");
  emit_pop("%rax");
  emit_add("%rcx", "%rax");
  emit_push("%rax");
}

void codegen(astree* ast) {
  if (ast->kind == AST_IDENT) {
    codegen_lvalue(ast);
    codegen_movevalue(ast->typ);
  } else if (ast->kind == AST_GLOBALREF) {
    codegen_lvalue(ast);
    codegen_movevalue(ast->typ);
  } else if (ast->kind == AST_INTLIT) {
    emit_push_int(ast->intval);
  } else if (ast->kind == AST_ADD) {
    codegen(ast->left);
    codegen(ast->right);
    codegen_add(ast->left->typ);
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
  } else if (ast->kind == AST_LESSEREQ) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("cmpq %%rcx, %%rax");
    emit_asm("setle %%al");
    emit_asm("movzbl %%al, %%eax");
    emit_push("%rax");
  } else if (ast->kind == AST_EQ) {
    codegen(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("cmpq %%rcx, %%rax");
    emit_asm("sete %%al");
    emit_asm("movzbl %%al, %%eax");
    emit_push("%rax");
  } else if (ast->kind == AST_LAND) {
    char* falsel = gen_label();
    char* endl = gen_label();
    codegen(ast->left);
    emit_pop("%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je %s", falsel);
    codegen(ast->right);
    emit_pop("%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("setne %%al");
    emit_push("%rax");
    emit_asm("jmp %s", endl);
    emit_label(falsel);
    emit_push("$0");
    emit_label(endl);
  } else if (ast->kind == AST_LOR) {
    char* truel = gen_label();
    char* endl = gen_label();
    codegen(ast->left);
    emit_pop("%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("jne %s", truel);
    codegen(ast->right);
    emit_pop("%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("setne %%al");
    emit_push("%rax");
    emit_asm("jmp %s", endl);
    emit_label(truel);
    emit_push("$1");
    emit_label(endl);
  } else if (ast->kind == AST_MINUS) {
    codegen(ast->value);
    emit_pop("%rax");
    emit_asm("negq %%rax");
    emit_push("%rax");
  } else if (ast->kind == AST_DOT) {
    codegen_lvalue(ast);
    codegen_movevalue(ast->typ);
  } else if (ast->kind == AST_ASSIGN) {
    codegen_lvalue(ast->left);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    codegen_movereg(ast->left->typ, "%cl", "%ecx", "%rcx");
  } else if (ast->kind == AST_ADDASSIGN) {
    codegen_lvalue(ast->left);
    emit_pop("%rax");
    emit_push("%rax");
    emit_push("%rax");
    codegen_movevalue(ast->left->typ);
    codegen(ast->right);
    codegen_add(ast->left->typ);
    emit_pop("%rcx");
    emit_pop("%rax");
    codegen_movereg(ast->value->typ, "%cl", "%ecx", "%rcx");
    emit_push("%rcx");
  } else if (ast->kind == AST_MULASSIGN) {
    codegen_lvalue(ast->left);
    emit_pop("%rax");
    emit_push("%rax");
    emit_push("%rax");
    codegen_movevalue(ast->left->typ);
    codegen(ast->right);
    emit_pop("%rcx");
    emit_pop("%rax");
    emit_asm("imulq %%rcx");
    emit_push("%rax");
    emit_pop("%rcx");
    emit_pop("%rax");
    codegen_movereg(ast->value->typ, "%cl", "%ecx", "%rcx");
    emit_push("%rcx");
  } else if (ast->kind == AST_PREINC) {
    codegen_lvalue(ast->value);
    emit_pop("%rax");
    emit_push("%rax");
    emit_push("%rax");
    codegen_movevalue(ast->value->typ);
    emit_push("$1");
    codegen_add(ast->value->typ);
    emit_pop("%rcx");
    emit_pop("%rax");
    codegen_movereg(ast->value->typ, "%cl", "%ecx", "%rcx");
    emit_push("%rcx");
  } else if (ast->kind == AST_ADDR) {
    codegen_lvalue(ast->value);
  } else if (ast->kind == AST_DEREF) {
    codegen(ast->value);
    codegen_movevalue(ast->typ);
  } else if (ast->kind == AST_VARDECL) {
    // discard
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    emit_push("%rdi");
    emit_push("%rsi");
    emit_push("%rdx");
    emit_push("%rcx");
    emit_push("%r8");
    emit_push("%r9");

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
    emit_asm("movq $0, %%rax");
    emit_call(ast->call->ident);
    if (ast->arguments->len-6 >= 1) {
      emit_asm("addq $%d, %%rsp", (ast->arguments->len-6)*8);
    }

    emit_pop("%r9");
    emit_pop("%r8");
    emit_pop("%rcx");
    emit_pop("%rdx");
    emit_pop("%rsi");
    emit_pop("%rdi");
    
    emit_push("%rax");
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<ast->stmt->len; i++) {
      astree* e = vector_get(ast->stmt, i);
      codegen(e);
    }
  } else if (ast->kind == AST_RETURN) {
    if (ast->value != NULL){
      codegen(ast->value);
      emit_pop("%rax");
    }
    emit_epilogue();
    emit_asm("ret");
  } else if (ast->kind == AST_IF) {
    codegen(ast->ifcond);
    char* endl = gen_label();
    char* nextl = gen_label();
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je %s", nextl);
    codegen(ast->ifbody);
    emit_asm("jmp %s", endl);
    for (int i=0; i<ast->elifconds->len; i++) {
      astree* cond = vector_get(ast->elifconds, i);
      astree* body = vector_get(ast->elifbodies, i);
      emit_label(nextl);
      codegen(cond);
      emit_asm("pop %%rax");
      emit_asm("cmpq $0, %%rax");
      nextl = gen_label();
      emit_asm("je %s", nextl);
      codegen(body);
      emit_asm("jmp %s", endl);
    }
    emit_label(nextl);
    if (ast->elsebody != NULL) {
      codegen(ast->elsebody);
    }
    emit_label(endl);
  } else if (ast->kind == AST_WHILE) {
    char* startl = gen_label();
    char* endl = gen_label();
    emit_label(startl);
    codegen(ast->whilecond);
    emit_asm("pop %%rax");
    emit_asm("cmpq $0, %%rax");
    emit_asm("je %s", endl);
    codegen(ast->whilebody);
    emit_asm("jmp %s", startl);
    emit_label(endl);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void codegen_funcdecl(funcdecl fdecl) {
  if (fdecl.body == NULL) return;

  emit_global(fdecl.fdecl->name);
  emit_label(fdecl.fdecl->name);
  emit_prologue(fdecl.stacksize);
  int argpos = 0;
  for (int i=0; i<fdecl.argdecls->len; i++) {
    paramtype* argparam = vector_get(fdecl.argdecls, i);
    int offset = argparam->offset;
    emit_asm("leaq -%d(%%rbp), %%rax", offset);
    if (i == 0) {
      codegen_movereg(argparam->typ, "%dil", "%edi", "%rdi");
    } else if (i == 1) {
      codegen_movereg(argparam->typ, "%sil", "%esi", "%rsi");
    } else if (i == 2) {
      codegen_movereg(argparam->typ, "%dl", "%edx", "%rdx");
    } else if (i == 3) {
      codegen_movereg(argparam->typ, "%cl", "%ecx", "%rcx");
    } else if (i == 4) {
      codegen_movereg(argparam->typ, "%r8b", "%r8d", "%r8");
    } else if (i == 5) {
      codegen_movereg(argparam->typ, "%r9b", "%r9d", "%r9");
    } else {
      argpos += 8;
      emit_asm("movq -%d(%%rbp), %%rcx", argpos);
      codegen_movereg(argparam->typ, "%cl", "%ecx", "%rcx");
    }
  }
  for (int i=0; i<fdecl.body->len; i++) {
    codegen(vector_get(fdecl.body, i));
  }
  emit_epilogue(fdecl.stacksize);
  emit_return();
}

void codegen_strlits() {
  printf(".data\n");
  for (int i=0; i<strlits->len; i++) {
    strlitinfo* info = vector_get(strlits, i);
    emit_label(info->label);
    emit_asm(".ascii \"%s\\0\"", info->strval);
  }
}

void codegen_toplevel(toplevel top) {
  if (top.kind == TOP_NONE) {
    // discard
  } else if (top.kind == TOP_FUNCDECL) {
    printf(".text\n");
    codegen_funcdecl(top.fdecl);
  } else if (top.kind == TOP_GLOBALVAR) {
    printf(".data\n");
    emit_label(top.vdecl->name);
    if (top.vinit != NULL) {
      if (top.vinit->kind == AST_INTLIT) {
        emit_asm(".int %d", top.vinit->intval);
      } else if (top.vinit->kind == AST_STRLIT) {
        emit_asm(".ascii \"%s\\0\"", top.vinit->strval);
      }
    } else {
      emit_asm(".zero %d", typesize(top.vdecl->typ));
    }
  } else if (top.kind == TOP_STRUCT) {
    // discard
  } else {
    assert(false);
  }
}
