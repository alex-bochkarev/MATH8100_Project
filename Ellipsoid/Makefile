flags = -g -O1 -larmadillo -lblas
src = ./ell-test.cpp
ofile= ./testell
logfile=./t.log
runargs=case0.data

.PHONY: all, compile, test

all: compile test

compile: ${src}
	g++ ${src} ${flags} -o ${ofile}

test:
	${ofile} ${runargs} > ${logfile}
