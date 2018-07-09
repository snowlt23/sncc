
CC = gcc
CFLAGS = -Wall

SNCC_LIBS = string.o vector.o lexer.o parser.o codegen.o

build: build-sncc ;

%o: %c
	$(CC) -c $(CFLAGS) %<

build-lexer-test: $(SNCC_LIBS)
	$(CC) $(CFLAGS) -olexer.out $(SNCC_LIBS) test/lexer_test.c
build-parser-test: $(SNCC_LIBS)
	$(CC) $(CFLAGS) -oparser.out $(SNCC_LIBS) test/parser_test.c
build-sncc: $(SNCC_LIBS)
	$(CC) $(CFLAGS) -osncc $(SNCC_LIBS) sncc.c

compiler-test:
	./compiler_test.sh
sncc-test:
	./sncc_test.sh

test: build-lexer-test build-parser-test build-sncc compiler-test sncc-test ;

clean:
	@rm -rf *.o *.s *.out sncc
