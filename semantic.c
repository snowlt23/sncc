#include <stdio.h>
#include <assert.h>
#include <string.h>
#include "sncc.h"

map* varmap;
int varpos;
map* fnmap;
map* globalvarmap;
vector* strlits;

int labelcnt = 0;

char* gen_label() {
  labelcnt++;
  char labelbuf[256];
  snprintf(labelbuf, 256, ".L%d", labelcnt);
  return strdup(labelbuf);
}

varinfo* new_varinfo(typenode* typ, int offset) {
  varinfo* info = malloc(sizeof(varinfo));
  info->typ = typ;
  info->offset = offset;
  return info;
}

strlitinfo* new_strlitinfo(char* label, char* strval) {
  strlitinfo* info = malloc(sizeof(strlitinfo));
  info->label = label;
  info->strval = strval;
  return info;
}

void init_semantic() {
  fnmap = new_map();
  globalvarmap = new_map();
}

void init_fn_semantic() {
  varmap = new_map();
  varpos = 0;
  strlits = new_vector();
}

int getalign(typenode* typ) {
  if (typ->truetype != NULL) {
    return getalign(typ->truetype);
  }

  if (typ->kind == TYPE_INT) {
    return 4;
  } else if (typ->kind == TYPE_UINT) {
    return 4;
  } else if (typ->kind == TYPE_CHAR) {
    return 1;
  } else if (typ->kind == TYPE_VOID) {
    error("void type hasn't align.");
  } else if (typ->kind == TYPE_PTR) {
    return 8;
  } else if (typ->kind == TYPE_ARRAY) {
    return getalign(typ->ptrof) * typ->arraysize;
  } else if (typ->kind == TYPE_STRUCT) {
    if (typ->maxalign == -1) error_s("%s is imcomplete type.", typ->tname);
    return typ->maxalign;
  } else {
    assert(false);
  }
}

int typesize(typenode* typ) {
  if (typ->truetype != NULL) {
    return typesize(typ->truetype);
  }

  if (typ->kind == TYPE_INCOMPLETE_STRUCT) error_s("%s type is incomplete type.", typ->tname);

  if (typ->kind == TYPE_INT) {
    return 4;
  } else if (typ->kind == TYPE_UINT) {
    return 4;
  } else if (typ->kind == TYPE_CHAR) {
    return 1;
  } else if (typ->kind == TYPE_VOID) {
    error("void type hasn't align.");
  } else if (typ->kind == TYPE_PTR) {
    return 8;
  } else if (typ->kind == TYPE_ARRAY) {
    return typesize(typ->ptrof) * typ->arraysize;
  } else if (typ->kind == TYPE_STRUCT) {
    if (typ->structsize == -1) error_s("%s is imcomplete type.", typ->tname);
    return typ->structsize;
  } else {
    assert(false);
  }
}

paramtype* get_field(typenode* typ, char* fieldname) {
  for (int i=0; i<typ->fields->len; i++) {
    paramtype* field = vector_get(typ->fields, i);
    if (strcmp(field->name, fieldname) == 0) {
      return field;
    }
  }
  error_ss("%s hasn't %s field.", typ->tname, fieldname);
}

bool is_implicit_int(typenode* typ) {
  return  typ->kind == TYPE_INT || typ->kind == TYPE_UINT ||typ->kind == TYPE_CHAR || typ->kind == TYPE_PTR;
}

void semantic_analysis(astree* ast) {
  if (ast->kind == AST_IDENT) {
    varinfo* info = map_get(varmap, ast->ident);
    if (info == NULL) { // lookup global variable
      typenode* typ = map_get(globalvarmap, ast->ident);
      if (typ == NULL) error_s("undeclared %s variable.", ast->ident);
      ast->kind = AST_GLOBALREF;
      ast->typ = typ;
    } else {
      ast->typ = info->typ;
      ast->offset = info->offset;
    }
  } else if (ast->kind == AST_INTLIT) {
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_STRLIT) {
    char* strval = ast->strval;
    int strsize = strlen(strval);
    ast->kind = AST_GLOBALREF;
    ast->ident = gen_label();
    ast->typ = new_arraynode(new_typenode(TYPE_CHAR), strsize+1);
    vector_push(strlits, new_strlitinfo(ast->ident, strval));
  } else if (ast->kind == AST_ADD || ast->kind == AST_SUB) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    if (ast->left->typ->kind == TYPE_PTR && ast->right->typ->kind == TYPE_INT) {
      ast->typ = ast->left->typ;
    } else if (ast->left->typ->kind == TYPE_UINT && is_implicit_int(ast->right->typ)) {
      ast->typ = ast->left->typ;
    } else if (is_implicit_int(ast->left->typ) && is_implicit_int(ast->right->typ)) {
      ast->typ = new_typenode(TYPE_INT);;
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MUL || ast->kind == AST_DIV || ast->kind == AST_MOD || ast->kind == AST_LESSER || ast->kind == AST_LESSEREQ) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    if (is_implicit_int(ast->left->typ) && is_implicit_int(ast->right->typ)) {
      ast->typ = new_typenode(TYPE_INT);;
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MINUS) {
    semantic_analysis(ast->value);
    ast->typ = ast->value->typ;
  } else if (ast->kind == AST_ASSIGN || ast->kind == AST_ADDASSIGN || ast->kind == AST_MULASSIGN) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_PREINC) {
    semantic_analysis(ast->value);
    ast->typ = ast->value->typ;
  } else if (ast->kind == AST_POSTINC) {
    ast->kind = AST_SUB;
    ast->left = new_ast_prefix(AST_PREINC, ast->value);
    ast->right = new_ast_intlit(1);
    semantic_analysis(ast);
  } else if (ast->kind == AST_EQ || ast->kind == AST_LAND || ast->kind == AST_LOR || ast->kind == AST_BAND) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
  } else if (ast->kind == AST_DOT) {
    semantic_analysis(ast->structvalue);
    if (ast->structvalue->typ->kind != TYPE_STRUCT) error("dot left expect struct value.");
    paramtype* field = get_field(ast->structvalue->typ, ast->fieldname);
    ast->typ = field->typ;
  } else if (ast->kind == AST_ADDR) {
    semantic_analysis(ast->value);
    ast->typ = new_ptrnode(ast->value->typ);
  } else if (ast->kind == AST_DEREF) {
    semantic_analysis(ast->value);
    if (ast->value->typ->kind != TYPE_PTR) error("cannot deref operator apply to noptr.");
    ast->typ = ast->value->typ->ptrof;
  } else if (ast->kind == AST_VARDECL) {
    varpos += typesize(ast->vardecl->typ);
    map_insert(varmap, ast->vardecl->name, new_varinfo(ast->vardecl->typ, varpos));
    if (ast->varinit != NULL) {
      ast->kind = AST_ASSIGN;
      ast->left = new_ast_ident(ast->vardecl->name);
      ast->right = ast->varinit;
      semantic_analysis(ast);
    }
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    for (int i=0; i<ast->arguments->len; i++) {
      semantic_analysis(vector_get(ast->arguments, i));
    }
    typenode* rettyp = map_get(fnmap, ast->call->ident);
    if (rettyp == NULL){
      warning_s("undeclared %s function.", ast->call->ident);
      ast->typ = new_typenode(TYPE_INT);
    } else {
      ast->typ = rettyp;
    }
  } else if (ast->kind == AST_STATEMENTS) {
    for (int i=0; i<ast->stmts->len; i++) {
      astree* e = vector_get(ast->stmts, i);
      semantic_analysis(e);
    }
  } else if (ast->kind == AST_RETURN) {
    if (ast->value != NULL) {
      semantic_analysis(ast->value);
    }
  } else if (ast->kind == AST_IF) {
    semantic_analysis(ast->ifcond);
    semantic_analysis(ast->ifbody);
    for (int i=0; i<ast->elifconds->len; i++) {
      astree* cond = vector_get(ast->elifconds, i);
      astree* body = vector_get(ast->elifbodies, i);
      semantic_analysis(cond);
      semantic_analysis(body);
    }
    if (ast->elsebody != NULL) {
      semantic_analysis(ast->elsebody);
    }
  } else if (ast->kind == AST_WHILE) {
    semantic_analysis(ast->whilecond);
    semantic_analysis(ast->whilebody);
  } else if (ast->kind == AST_FOR) {
    semantic_analysis(ast->forinit);
    semantic_analysis(ast->forcond);
    semantic_analysis(ast->fornext);
    semantic_analysis(ast->forbody);
  } else if (ast->kind == AST_BREAK || ast->kind == AST_CONTINUE) {
    // discard
  } else if (ast->kind == AST_SIZEOF_EXPR) {
    semantic_analysis(ast->value);
    ast->kind = AST_INTLIT;
    ast->intval = typesize(ast->value->typ);
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_SIZEOF_TYPE) {
    ast->kind = AST_INTLIT;
    ast->intval = typesize(ast->typedesc);
    ast->typ = new_typenode(TYPE_INT);
  } else {
    error("unsupported kind in semantic");
  }
}

void semantic_analysis_toplevel(toplevel* top) {
  init_fn_semantic();
  if (top->kind == TOP_NONE) {
    // discard
  } else if (top->kind == TOP_FUNCDECL) {
    map_insert(fnmap, top->fdecl->name, top->fdecl->typ);
    for (int i=0; i<top->argdecls->len; i++) {
      paramtype* argparam = vector_get(top->argdecls, i);
      varpos += typesize(argparam->typ);
      argparam->offset = varpos;
      map_insert(varmap, argparam->name, new_varinfo(argparam->typ, varpos));
    }
    if (top->body != NULL) {
      for (int i=0; i<top->body->len; i++) {
        semantic_analysis(vector_get(top->body, i));
      }
      top->stacksize = varpos;
    }
  } else if (top->kind == TOP_EXTERN) {
    map_insert(globalvarmap, top->vdecl->name, top->vdecl->typ);
  } else if (top->kind == TOP_GLOBALVAR) {
    map_insert(globalvarmap, top->vdecl->name, top->vdecl->typ);
    if (top->vinit != NULL) {
      if (top->vinit->kind != AST_INTLIT && top->vinit->kind != AST_STRLIT && !(top->vinit->kind == AST_MINUS && top->vinit->value->kind == AST_INTLIT)) {
        error("global variable expected constant expression.");
      }
      if (top->vinit->kind == AST_STRLIT) {
        top->vdecl->typ->truetype = new_typenode(TYPE_ARRAY);
        top->vdecl->typ->truetype->ptrof = new_typenode(TYPE_CHAR);
        top->vdecl->typ->truetype->arraysize = strlen(top->vinit->strval) + 1;
      }
    }
  } else if (top->kind == TOP_STRUCT) {
    // decide typesize
    int maxalign = 0;
    for (int i=0; i<top->structtype->fields->len; i++) {
      paramtype* field = vector_get(top->structtype->fields, i);
      if (maxalign < getalign(field->typ)) {
        maxalign = getalign(field->typ);
      }
    }
    int alignsize = 0;
    int structsize = 0;
    for (int i=0; i<top->structtype->fields->len; i++) {
      paramtype* field = vector_get(top->structtype->fields, i);
      if (alignsize == maxalign) {
        alignsize = 0;
      } else if (alignsize+typesize(field->typ) > maxalign) {
        structsize += maxalign - alignsize % maxalign;
        alignsize = 0;
      }
      field->offset = structsize;
      structsize += typesize(field->typ);
      alignsize += typesize(field->typ);
    }
    if (alignsize != maxalign) {
      structsize += maxalign - alignsize % maxalign;
    }
    top->structtype->structsize = structsize;
    top->structtype->maxalign = maxalign;
  } else {
    assert(false);
  }
}
