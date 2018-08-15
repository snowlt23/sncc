CC = gcc
CFLAGS = -Wall

SNCC_LIBS = vector.o map.o lexer.o parser.o semantic.o codegen.o
SELF_LIBS = vector.self.o map.self.o lexer.o parser.o semantic.o codegen.o

build: sncc ;

%.o: %.c sncc.h
	$(CC) -c $(CFLAGS) $<
%.self.o: %.self.c sncc.h
	./sncc < $< > $(basename $<).s
	$(CC) -c $(basename $<).s

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

test.pp.c: test.c
	gcc -P -E test.c > test.pp.c
test: compiler-test sncc test.pp.c
	./sncc < test.pp.c > test.s
	gcc test.s
	./a.out

self: sncc $(SELF_LIBS) sncc.c
	$(CC) $(CFLAGS) -oself $(SELF_LIBS) sncc.c
self-test: self test.pp.c
	./self < test.pp.c > test.s
	gcc test.s
	./a.out

.PHONY: clean
clean:
	@rm -rf *.o *.s *.out sncc
