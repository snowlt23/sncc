
build:
	gcc -osncc -Wall sncc.c

compiler-test:
	./compiler_test.sh
sncc-test:
	./sncc_test.sh

test: build compiler-test sncc-test ;
