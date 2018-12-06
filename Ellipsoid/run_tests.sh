#!/bin/bash
logf=logs/tests.log

echo Tests run: $(date "+%Y-%m-%d %H:%M:%S") > $logf
printf " +----------------------+-------------------+-----------------+------------+---------+\n" >> $logf
printf " | Test case            | Optimal objective | Found objective | Difference | Result  |\n" >> $logf
printf " +----------------------+-------------------+-----------------+------------+---------+\n" >> $logf
for file in $(ls ./data/testcase*.data); do
		fname=./logs/$(basename $file .data).log
    echo $(date "+%Y-%m-%d %H:%M:%S") > $fname
		$1 $file >> $fname;
    casen=$(basename $file .data)
    fObj=$(cat $fname | sed -n '/^Optimal objective (found): /p' | sed -r 's/Optimal objective \(found\): (.+)/\1/')
    res=$(cat $fname | sed -n '/^SOLUTION CHECK:/p' | sed -r 's/SOLUTION CHECK: (.+)/\1/')
    oObj=$(cat $fname | sed -n '/^Optimal objective (known): /p' | sed -r 's/Optimal objective \(known\): (.+)/\1/')
    oGap=$(cat $fname | sed -n '/^Objective value error:/p' | sed -r 's/Objective value error: (.+)/\1/')
    printf " | %-20s | %17s | %15s | %-10s | %-7s |\n" "$casen" "$oObj" "$fObj" "$oGap" "$res" >> $logf
	  done

printf " +----------------------+-------------------+-----------------+------------+---------+\n" >> $logf


cat logs/tests.log
