#!/bin/bash
logf=logs/kmc_tests.log

echo Klee-Minty cubes > $logf
echo Tests run: $(date "+%Y-%m-%d %H:%M:%S") > $logf
printf " +-----+----------------------+-----------+-------------------------+---------------------------+------------+---------+\n" >> $logf
printf " | No. |Test case             | Wall time | Opt.objective           | Found objective           | Difference | Result  |\n" >> $logf
printf " +-----+----------------------+-----------+-------------------------+---------------------------+------------+---------+\n" >> $logf

counter=1

for file in $(ls ./data/KMC_*.data); do
		fname=./logs/$(basename $file .data).log
    echo $(date "+%Y-%m-%d %H:%M:%S") > $fname
		$1 $file --verbose >> $fname;
    casen=$(basename $file .data)
    fObj=$(cat $fname | sed -n '/^Optimal objective (found): /p' | sed -r 's/Optimal objective \(found\): (.+)/\1/')
    res=$(cat $fname | sed -n '/^SOLUTION CHECK:/p' | sed -r 's/SOLUTION CHECK: (.+)/\1/')
    oObj=$(cat $fname | sed -n '/^Optimal objective (known): /p' | sed -r 's/Optimal objective \(known\): (.+)/\1/')
    oGap=$(cat $fname | sed -n '/^Objective value error:/p' | sed -r 's/Objective value error: (.+)/\1/')
    wTime=$(cat $fname | sed -n '/Wall Clock/p' | sed -r 's/.+Wall Clock]: (.+)/\1/')
    printf " | %-3s | %-20s | %-9s | %23s | %25s | %-10s | %-7s |\n" "$counter" "$casen" "$wTime" "$oObj" "$fObj" "$oGap" "$res" >> $logf
    ((counter++))
	  done

printf " +-----+----------------------+-----------+-------------------------+---------------------------+------------+---------+\n" >> $logf
cat logs/kmc_tests.log
