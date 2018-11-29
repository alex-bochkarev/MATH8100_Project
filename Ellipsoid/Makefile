flags = -g -O1 -larmadillo -lblas
src = ./ell-test.cpp
ofile= ./testell
.PHONY: all, compile, tests

all: compile tests

compile: ${src}
	g++ ${src} ${flags} -o ${ofile}

tests: compile
	./run_tests.sh ${ofile}
.ONESHELL:
