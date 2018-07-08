
build:
	gcc -osncc sncc.c

compiler-test:
	./compiler_test.sh
sncc-test:
	./sncc_test.sh

test: compiler-test sncc-test
