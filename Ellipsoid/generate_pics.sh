#!/bin/bash

## compile everything
make compile

## generate pictures for the figure 1 (Ellipsoid steps)
# generate logfiles
./testell data/testcase-2d.data --verbose > logs/2d-for-pic.log

# generate graphs

# see vlogParser.R file

# bash to summarize logs
cat logs/tests.log | grep FAILED # failed cases
cat logs/tests.log | grep +INF | wc -l #infeasible cases
cat logs/tests.log | grep -ve"INF" | grep -e"| [0-9]" # not unfeasible or unbounded
