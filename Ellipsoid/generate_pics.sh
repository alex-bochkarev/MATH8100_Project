#!/bin/bash

## compile everything
make compile

## generate pictures for the figure 1 (Ellipsoid steps)
# generate logfiles
./testell data/testcase-2d.data --verbose > logs/2d-for-pic.log

# generate graphs

# <INSERT R script here>
