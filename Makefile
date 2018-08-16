CC = gcc
CFLAGS = -Wall -Wextra -O2

SNCC_LIBS = vector.o map.o lexer.o parser.o semantic.o codegen.o utils.o
SELF_LIBS = vector.self.o map.self.o lexer.self.o parser.self.o semantic.self.o codegen.self.o utils.self.o selflib.self.o
SELF2_LIBS = vector.self2.o map.self2.o lexer.self2.o parser.self2.o semantic.self2.o codegen.self2.o utils.self2.o selflib.self2.o

build: sncc ;

%.o: %.c sncc.h
	$(CC) -c $(CFLAGS) $<
%.self.o: %.c sncc.h
	./sncc < $< > $(basename $<).self.s
	$(CC) -c $(CFLAGS) $(basename $<).self.s
%.self2.o: %.c sncc.h
	./self < $< > $(basename $<).self2.s
	$(CC) -c $(CFLAGS) $(basename $<).self2.s

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
sncc-test: compiler-test sncc test.pp.c
	./sncc < test.pp.c > test.s
	gcc test.s sncclib.c
	./a.out

self: sncc $(SELF_LIBS) sncc.self.o
	$(CC) -oself $(SELF_LIBS) sncc.self.o
self-test: self test.pp.c
	./self < test.pp.c > test.s
	gcc test.s sncclib.c
	./a.out

self2: self $(SELF2_LIBS) sncc.self2.o
	 $(CC) -oself2 $(SELF2_LIBS) sncc.self2.o
self2-test: self2 test.pp.c
	./self2 < test.pp.c > test.s
	gcc test.s sncclib.c
	./a.out

test: sncc-test self-test self2-test ;

.PHONY: clean
clean:
	@rm -rf *.o *.s *.out sncc self self2 test.pp.c
