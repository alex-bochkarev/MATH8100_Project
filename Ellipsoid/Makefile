flags = -g -O1 -larmadillo -lblas
src = ./ell-test.cpp
ofile= ./testell
logfile=./t.log
runargs=A.data b.data c.data 0.01

.PHONY: all, compile, test

all: compile test

compile: ${src}
	g++ ${src} ${flags} -o ${ofile}

test:
	${ofile} ${runargs} > ${logfile}
