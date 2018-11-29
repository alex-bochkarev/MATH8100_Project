flags = -g -O1 -larmadillo -lblas
src = ./ell-test.cpp
ofile= ./testell
logfile=./caseMult.log
runargs=caseMult.data

.PHONY: all, compile, tests

all: compile tests

compile: ${src}
	g++ ${src} ${flags} -o ${ofile}

tests:
	${ofile} ${runargs} > ${logfile}
