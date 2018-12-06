flags = -g -O1 -larmadillo -lblas
genlogs = logs/gen.log
src = ./ell-test.cpp
tgen = ./generate_tests.cpp
otgen = ./testgen
ofile= ./testell
GINC = $$(dirname $$(locate gurobi_c++.h))
GFLAGS = -m64 -g
CPPLIB = -L$$(dirname $$(locate libgurobi_c++)) -lgurobi_c++ -lgurobi81 -larmadillo -lblas -lboost_program_options
CPP = g++

.PHONY: all, compile, tests, testgen, randcases, randtests

all: compile testgen tests

compile: ${src}
	${CPP} ${src} ${flags} -o ${ofile}

tests:
	./run_tests.sh ${ofile}

testgen: ${tgen}
	${CPP} ${tgen} -I${GINC} -o ${otgen} ${CPPLIB} -lm

randcases:
	rm data/randcase*
	${otgen} -n 10 -m 5 -N 50 --filename data/randcaseF > ${genlogs}
	${otgen} -n 10 -m 5 -N 50 --nonnegative yes --filename data/randcaseNN >> ${genlogs}
	${otgen} -n 10 -m 10 -N 50 --filename data/randcaseF >> ${genlogs}
	${otgen} -n 10 -m 10 -N 50 --nonnegative yes --filename data/randcaseNN >> ${genlogs}
	${otgen} -n 10 -m 15 -N 50 --filename data/randcaseF >> ${genlogs}
	${otgen} -n 10 -m 15 -N 50 --nonnegative yes --filename data/randcaseNN >> ${genlogs}


randtests:
	./run_rtests.sh ${ofile}

.ONESHELL:
