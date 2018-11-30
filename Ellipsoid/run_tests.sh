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
    fObj=$(cat $fname | sed -n '/^Objective at the point found:/p' | sed -r 's/Objective at the point found:(.+)/\1/')
    res=$(cat $fname | sed -n '/^SOLUTION CHECK:/p' | sed -r 's/SOLUTION CHECK: (.+)/\1/')
    oObj=$(cat $fname | sed -n '/^Objective at the known opt point/p' | sed -r 's/Objective at the known opt point: (.+)/\1/')
    oGap=$(cat $fname | sed -n '/^Optimality gap:/p' | sed -r 's/Optimality gap:(.+)/\1/')
    printf " | %-20s | %17s | %15s | %-10s | %-7s |\n" "$casen" "$(printf "%0.2f" $oObj)" "$(printf "%0.2f" $fObj)" "$(printf "%0.4f" $oGap)" "$res" >> $logf
	  done

printf " +----------------------+-------------------+-----------------+------------+---------+\n" >> $logf


cat logs/tests.log
