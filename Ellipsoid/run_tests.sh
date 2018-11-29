#!/bin/bash

echo $(date "+%Y-%m-%d %H:%M:%S") > logs/tests.log

for file in $(ls ./data/testcase*.data); do
		fname=./logs/$(basename $file .data).log
    echo $(date "+%Y-%m-%d %H:%M:%S") > $fname
		$1 $file >> $fname;
    echo $(basename $file .data) -- $(cat $fname | sed -n '/^SOLUTION CHECK:/p' | sed -r 's/SOLUTION CHECK: (.+)/\1/') >> logs/tests.log
	  done

cat logs/tests.log
