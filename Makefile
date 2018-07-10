CC = gcc
CFLAGS = -Wall

SNCC_LIBS = string.o vector.o map.o lexer.o parser.o codegen.o

build: sncc ;

%.o: %.c sncc.h
	$(CC) -c $(CFLAGS) $<

lexer.out: $(SNCC_LIBS) sncc.h test/lexer_test.c
	$(CC) $(CFLAGS) -olexer.out $(SNCC_LIBS) test/lexer_test.c
parser_expr.out: $(SNCC_LIBS) sncc.h test/parser_expr_test.c
	$(CC) $(CFLAGS) -oparser_expr.out $(SNCC_LIBS) test/parser_expr_test.c
parser_stmt.out: $(SNCC_LIBS) sncc.h test/parser_stmt_test.c
	$(CC) $(CFLAGS) -oparser_stmt.out $(SNCC_LIBS) test/parser_stmt_test.c
funcdecl.out: $(SNCC_LIBS) sncc.h test/funcdecl_test.c
	$(CC) $(CFLAGS) -ofuncdecl.out $(SNCC_LIBS) test/funcdecl_test.c
sncc: $(SNCC_LIBS) sncc.c
	$(CC) $(CFLAGS) -osncc $(SNCC_LIBS) sncc.c

compiler-test: lexer.out parser_expr.out parser_stmt.out funcdecl.out
	./compiler_test.sh
sncc-test:
	./sncc_test.sh

test: sncc compiler-test sncc-test ;

.PHONY: clean
clean:
	@rm -rf *.o *.s *.out sncc
