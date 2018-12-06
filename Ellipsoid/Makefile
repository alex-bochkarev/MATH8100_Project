flags = -g -O1 -larmadillo -lblas
src = ./ell-test.cpp
tgen = ./generate_tests.cpp
otgen = ./testgen
ofile= ./testell
GINC = $$(dirname $$(locate gurobi_c++.h))
GFLAGS = -m64 -g
CPPLIB = -L$$(dirname $$(locate libgurobi_c++)) -lgurobi_c++ -lgurobi81 -larmadillo -lblas -lboost_program_options

CPP = g++
.PHONY: all, compile, tests, testgen

all: compile testgen tests

compile: ${src}
	${CPP} ${src} ${flags} -o ${ofile}

tests:
	./run_tests.sh ${ofile}

testgen:
	${CPP} ${tgen} -I${GINC} -o ${otgen} ${CPPLIB} -lm

.ONESHELL:
