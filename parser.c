#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sncc.h"

bool eq_ident(token* id, char* s) {
  return id != NULL && id->kind == TOKEN_IDENT && strcmp(id->ident, s) == 0;
}

void expect_token(token* t, tokenkind kind) {
  if (t == NULL || t->kind != kind) error("expected %d token.", kind);
}

//
// ast
//

astree* new_ast(astkind kind) {
  astree* ast = (astree*)malloc(sizeof(astree));
  ast->kind = kind;
  ast->typ = NULL;
  return ast;
}

astree* new_ast_prefix(astkind kind, astree* value) {
  astree* ast = new_ast(kind);
  ast->value = value;
  return ast;
}

astree* new_ast_infix(astkind kind, astree* left, astree* right) {
  astree* ast = new_ast(kind);
  ast->left = left;
  ast->right = right;
  return ast;
}

astree* new_ast_intlit(int x) {
  astree* ast = new_ast(AST_INTLIT);
  ast->intval = x;
  return ast;
}

astree* new_ast_strlit(char* s) {
  astree* ast = new_ast(AST_STRLIT);
  ast->strval = s;
  return ast;
}

astree* new_ast_ident(char* id) {
  astree* ast = new_ast(AST_IDENT);
  ast->ident = id;
  return ast;
}

char* ast_to_kindstr(astree* ast) {
  if (ast->kind == AST_ADD) {
    return "AST_ADD";
  } else if (ast->kind == AST_SUB) {
    return "AST_SUB";
  } else if (ast->kind == AST_MUL) {
    return "AST_MUL";
  } else if (ast->kind == AST_DIV) {
    return "AST_DIV";
  } else if (ast->kind == AST_MINUS) {
    return "AST_MINUS";
  } else if (ast->kind == AST_LESSER) {
    return "AST_LESSER";
  } else if (ast->kind == AST_ASSIGN) {
    return "AST_ASSIGN";
  } else if (ast->kind == AST_ADDR) {
    return "AST_ADDR";
  } else if (ast->kind == AST_DEREF) {
    return "AST_DEREF";
  } else if (ast->kind == AST_VARDECL) {
    return "AST_VARDECL";
  } else if (ast->kind == AST_CALL) {
    return "AST_CALL";
  } else if (ast->kind == AST_STATEMENT) {
    return "AST_STATEMENT";
  } else if (ast->kind == AST_INTLIT) {
    return "AST_INTLIT";
  } else if (ast->kind == AST_STRLIT) {
    return "AST_STRLIT";
  } else if (ast->kind == AST_IDENT) {
    return "AST_IDENT";
  } else if (ast->kind == AST_RETURN) {
    return "AST_RETURN";
  } else if (ast->kind == AST_IF) {
    return "AST_IF";
  } else if (ast->kind == AST_WHILE) {
    return "AST_WHILE";
  } else {
    assert(0);
  }
}

//
// typenode
//

typenode* new_typenode(typekind kind) {
  typenode* tn = malloc(sizeof(typenode));
  tn->kind = kind;
  tn->ptrof = NULL;
  tn->truetype = NULL;
  return tn;
}

typenode* new_ptrnode(typenode* typ) {
  typenode* tn = new_typenode(TYPE_PTR);
  tn->ptrof = typ;
  return tn;
}

typenode* new_arraynode(typenode* typ, size_t size) {
  typenode* tn = new_typenode(TYPE_PTR);
  tn->ptrof = typ;
  tn->truetype = new_typenode(TYPE_ARRAY);
  tn->truetype->ptrof = typ;
  tn->truetype->arraysize = size;
  return tn;
}

//
// tokenstream
//

tokenstream* new_tokenstream(vector* tokens) {
  tokenstream* ts = (tokenstream*)malloc(sizeof(tokenstream));
  ts->tokens = tokens;
  ts->pos = 0;
  return ts;
}
token* get_token(tokenstream* ts) {
  if (ts->pos >= ts->tokens->len) {
    return NULL;
  }
  return (token*)vector_get(ts->tokens, ts->pos);
}
void next_token(tokenstream* ts) {
  ts->pos++;
}

//
// parser
//

typenode* parse_type(tokenstream* ts) {
  typenode* tn;
  if (eq_ident(get_token(ts), "int")) {
    next_token(ts);
    tn = new_typenode(TYPE_INT);
  } else if (eq_ident(get_token(ts), "char")) {
    next_token(ts);
    tn = new_typenode(TYPE_CHAR);
  } else {
    return NULL;
  }
  for (;;) {
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_MUL) {
      next_token(ts);
      tn = new_ptrnode(tn);
    } else {
      break;
    }
  }
  return tn;
}

astree* parser_top(tokenstream* ts);

astree* factor(tokenstream* ts) {
  token* t = get_token(ts);
  if (t == NULL) error("require more token");

  if (t->kind == TOKEN_LPAREN) {
    next_token(ts);
    astree* ret = expression(ts);
    token* t = get_token(ts);
    if (t == NULL || t->kind != TOKEN_RPAREN) error("\"(\" should be ended by \")\".");
    next_token(ts);
    return ret;
  } else if (t->kind == TOKEN_INTLIT) {
    next_token(ts);
    return new_ast_intlit(t->intval);
  } else if (t->kind == TOKEN_STRLIT) {
    next_token(ts);
    return new_ast_strlit(t->strval);
  } else if (t->kind == TOKEN_IDENT) {
    next_token(ts);
    return new_ast_ident(t->ident);
  } else {
    return expression(ts);
  }
}

astree* gen_ptrderef(astree* value, astree* index) {
  return new_ast_prefix(AST_DEREF, new_ast_infix(AST_ADD, value, index));
}

astree* arrayref(tokenstream* ts) {
  astree* value = factor(ts);
  for (;;) {
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_LBRACKET) {
      next_token(ts);
      astree* index = expression(ts);
      value = gen_ptrderef(value, index);
      expect_token(get_token(ts), TOKEN_RBRACKET); next_token(ts);
    } else {
      break;
    }
  }
  return value;
}

astree* callexpr(tokenstream* ts) {
  astree* call = arrayref(ts);
  token* lparen = get_token(ts);
  if (lparen == NULL) return call;
  if (lparen->kind != TOKEN_LPAREN) return call;
  next_token(ts);

  vector* args = new_vector();
  if (get_token(ts)-> kind != TOKEN_RPAREN) {
    vector_push(args, expression(ts));
    for (;;) {
      if (get_token(ts)->kind == TOKEN_RPAREN) break;
      token* t = get_token(ts); next_token(ts);
      if (t->kind != TOKEN_COMMA) error("expected comma by call, but got %s", token_to_str(t));
      vector_push(args, expression(ts));
    }
  }

  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("function call expected )rparen.");

  astree* ast = new_ast(AST_CALL);
  ast->call = call;
  ast->arguments = args;
  return ast;
}

// infix parser for lesser, greater, and lesser/greater equal.
astree* infix_lge(tokenstream* ts) {
  astree* left = callexpr(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_LESSER) {
      next_token(ts);
      astree* right = callexpr(ts);
      left = new_ast_infix(AST_LESSER, left, right);
    } else if (t->kind == TOKEN_GREATER) {
      next_token(ts);
      astree* right = callexpr(ts);
      left = new_ast_infix(AST_LESSER, right, left);
    } else {
      break;
    }
  }
  return left;
}

astree* infix_muldiv(tokenstream* ts) {
  astree* left = infix_lge(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_MUL) {
      next_token(ts);
      astree* right = infix_lge(ts);
      left = new_ast_infix(AST_MUL, left, right);
    } else if (t->kind == TOKEN_DIV) {
      next_token(ts);
      astree* right = infix_lge(ts);
      left = new_ast_infix(AST_DIV, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* infix_addsub(tokenstream* ts) {
  astree* left = infix_muldiv(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ADD) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_ADD, left, right);
    } else if (t->kind == TOKEN_SUB) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_SUB, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* prefix_addsub_deref(tokenstream* ts) {
  token* t = get_token(ts);
  if (t != NULL && t->kind == TOKEN_SUB) {
    next_token(ts);
    astree* value = prefix_addsub_deref(ts);
    return new_ast_prefix(AST_MINUS, value);
  } else if (t != NULL && t->kind == TOKEN_MUL) {
    next_token(ts);
    astree* value = prefix_addsub_deref(ts);
    return new_ast_prefix(AST_DEREF, value);
  } else if (t != NULL && t->kind == TOKEN_AND) {
    next_token(ts);
    astree* value = prefix_addsub_deref(ts);
    return new_ast_prefix(AST_ADDR, value);
  } else {
    return infix_addsub(ts);
  }
}

astree* prefix_sizeof(tokenstream* ts) {
  if (eq_ident(get_token(ts), "sizeof")) {
    next_token(ts);
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_LPAREN) next_token(ts);
    astree* ast;
    typenode* typ = parse_type(ts);
    if (typ == NULL) {
      ast = new_ast(AST_SIZEOF_EXPR);
      ast->value = expression(ts);
    } else {
      ast = new_ast(AST_SIZEOF_TYPE);
      ast->typedesc = typ;
    }
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_RPAREN) next_token(ts);
    return ast;
  } else {
    return prefix_addsub_deref(ts);
  }
}

astree* infix_assign(tokenstream* ts) {
  astree* left = prefix_sizeof(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ASSIGN) {
      next_token(ts);
      astree* right = prefix_sizeof(ts);
      left = new_ast_infix(AST_ASSIGN, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* expression(tokenstream* ts) {
  return infix_assign(ts);
}

astree* parse_return(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "return")) return NULL;
  next_token(ts);

  astree* ast = new_ast(AST_RETURN);
  ast->value = expression(ts);
  expect_token(get_token(ts), TOKEN_SEMICOLON); next_token(ts);

  return ast;
}

astree* parse_if(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "if")) return NULL;
  next_token(ts);

  astree* ast = new_ast(AST_IF);

  expect_token(get_token(ts), TOKEN_LPAREN); next_token(ts);
  ast->ifcond = expression(ts);
  expect_token(get_token(ts), TOKEN_RPAREN); next_token(ts);
  ast->ifbody = parse_compound(ts);

  ast->elsebody = NULL;
  if (eq_ident(get_token(ts), "else")) {
    next_token(ts);
    ast->elsebody = parse_compound(ts);
  }

  return ast;
}

astree* parse_while(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "while")) return NULL;
  next_token(ts);

  astree* ast = new_ast(AST_WHILE);

  expect_token(get_token(ts), TOKEN_LPAREN); next_token(ts);
  ast->whilecond = expression(ts);
  expect_token(get_token(ts), TOKEN_RPAREN); next_token(ts);
  ast->whilebody = parse_compound(ts);

  return ast;
}

astree* parse_for(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "for")) return NULL;
  next_token(ts);

  expect_token(get_token(ts), TOKEN_LPAREN); next_token(ts);

  astree* forinit = expression(ts);
  expect_token(get_token(ts), TOKEN_SEMICOLON); next_token(ts);

  astree* forcond = expression(ts);
  expect_token(get_token(ts), TOKEN_SEMICOLON); next_token(ts);

  astree* fornext = expression(ts);

  expect_token(get_token(ts), TOKEN_RPAREN); next_token(ts);

  astree* forbody = parse_compound(ts);
  astree* bodyast = new_ast(AST_STATEMENT);
  bodyast->stmt = new_vector();
  vector_push(bodyast->stmt, (void*)forbody);
  vector_push(bodyast->stmt, (void*)fornext);

  astree* whileast = new_ast(AST_WHILE);
  whileast->whilecond = forcond;
  whileast->whilebody = bodyast;

  astree* ast = new_ast(AST_STATEMENT);
  ast->stmt = new_vector();
  vector_push(ast->stmt, (void*)forinit);
  vector_push(ast->stmt, (void*)whileast);

  return ast;
}

astree* parse_vardecl(tokenstream* ts) {
  paramtype* pt = parse_paramtype(ts);
  if (pt == NULL) return NULL;
  astree* ast = new_ast(AST_VARDECL);
  ast->vardecl = pt;
  return ast;
}

astree* parse_statement(tokenstream* ts) {
  astree* returnast = parse_return(ts);
  if (returnast != NULL) {
    return returnast;
  }

  astree* ifast = parse_if(ts);
  if (ifast != NULL) {
    return ifast;
  }

  astree* whileast = parse_while(ts);
  if (whileast != NULL) {
    return whileast;
  }

  astree* forast = parse_for(ts);
  if (forast != NULL) {
    return forast;
  }

  astree* declast = parse_vardecl(ts);
  if (declast != NULL) {
    return declast;
  }

  astree* e = expression(ts);
  if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_SEMICOLON) next_token(ts);
  return e;
}

vector* parse_statements(tokenstream* ts) {
  vector* v = new_vector();
  for (;;) {
    if (get_token(ts) == NULL || get_token(ts)->kind == TOKEN_RBRACE) break;
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_SEMICOLON) {
      next_token(ts);
      continue;
    }
    vector_push(v, (void*)parse_statement(ts));
    // vector_push(v, (void*)parse_compound(ts));
  }
  return v;
}

astree* parse_compound(tokenstream* ts) {
  if (get_token(ts) == NULL) return NULL;
  if (get_token(ts)->kind == TOKEN_LBRACE) {
    next_token(ts);
    astree* ast = new_ast(AST_STATEMENT);
    ast->stmt = parse_statements(ts);
    expect_token(get_token(ts), TOKEN_RBRACE); next_token(ts);
    return ast;
  } else {
    astree* ast = parse_statement(ts);
    return ast;
  }
}

//
// funcdecl
//

paramtype* parse_paramtype(tokenstream* ts) {
  typenode* tn = parse_type(ts);
  if (tn == NULL) return NULL;
  token* t = get_token(ts); next_token(ts);
  if (t->kind != TOKEN_IDENT) error("expected identifier in parameter.");
  for (;;) { // array declaration
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_LBRACKET) {
      next_token(ts);
      expect_token(get_token(ts), TOKEN_INTLIT);
      int arraysize = get_token(ts)->intval;
      next_token(ts);
      if (tn->kind == TYPE_ARRAY) {
        tn->ptrof = new_arraynode(tn->ptrof, arraysize);
      } else {
        tn = new_arraynode(tn, arraysize);
      }
      expect_token(get_token(ts), TOKEN_RBRACKET); next_token(ts);
    } else {
      break;
    }
  }
  paramtype* pt = malloc(sizeof(paramtype));
  pt->typ = tn;
  pt->name = t->ident;
  return pt;
}

vector* parse_paramtype_list(tokenstream* ts) {
  vector* ptlist = new_vector();
  for (;;) {
    if (get_token(ts) == NULL) break;
    if (get_token(ts)->kind != TOKEN_IDENT) break;
    paramtype* pt = parse_paramtype(ts);
    if (pt == NULL) error("expect type declaration in parameters.");
    vector_push(ptlist, (void*)pt);
    token* t = get_token(ts);
    if (t == NULL) error("require more token.");
    if (t->kind != TOKEN_COMMA) break;
    next_token(ts);
  }
  return ptlist;
}

toplevel parse_toplevel(tokenstream* ts) {
  toplevel top;

  paramtype* pt = parse_paramtype(ts);
  if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_LPAREN) {
    next_token(ts);
    top.kind = TOP_FUNCDECL;
    top.fdecl.fdecl = pt;
    top.fdecl.argdecls = parse_paramtype_list(ts);
    expect_token(get_token(ts), TOKEN_RPAREN); next_token(ts);
    expect_token(get_token(ts), TOKEN_LBRACE); next_token(ts);
    top.fdecl.body = parse_statements(ts);
    expect_token(get_token(ts), TOKEN_RBRACE); next_token(ts);
  } else {
    top.kind = TOP_GLOBALVAR;
    top.vdecl = pt;
  }

  if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_SEMICOLON) next_token(ts);

  return top;
}
