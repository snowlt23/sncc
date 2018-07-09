CC = gcc
CFLAGS = -Wall

SNCC_LIBS = string.o vector.o map.o lexer.o parser.o codegen.o

build: sncc ;

%.o: %.c sncc.h
	$(CC) -c $(CFLAGS) $<

lexer.out: $(SNCC_LIBS) sncc.h
	$(CC) $(CFLAGS) -olexer.out $(SNCC_LIBS) test/lexer_test.c
parser.out: $(SNCC_LIBS) sncc.h
	$(CC) $(CFLAGS) -oparser.out $(SNCC_LIBS) test/parser_test.c
funcdecl.out: $(SNCC_LIBS) sncc.h
	$(CC) $(CFLAGS) -ofuncdecl.out $(SNCC_LIBS) test/funcdecl_test.c
sncc: $(SNCC_LIBS)
	$(CC) $(CFLAGS) -osncc $(SNCC_LIBS) sncc.c

compiler-test: lexer.out parser.out funcdecl.out
	./compiler_test.sh
sncc-test:
	./sncc_test.sh

test: sncc compiler-test sncc-test ;

.PHONY: clean
clean:
	@rm -rf *.o *.s *.out sncc
