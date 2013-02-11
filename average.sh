#!/bin/sh

#file has to have 3 columns with 3 different algorithms
#the first line does not contain numbers

file=$1
if [ -z $file ]; then
	echo "provide a filename!"
	exit 1
fi

echo -e "algo\taverage\t\tdeviation"
echo -ne "opt\t"
awk '{sum+=$3; sumsq+=$3*$3} END {print sum/(NR-1) "\t\t" sqrt(sumsq/(NR-1) - (sum/(NR-1))**2)}' $1


echo -ne "new\t"
awk '{sum+=$4; sumsq+=$4*$4} END {print sum/(NR-1) "\t\t" sqrt(sumsq/(NR-1) - (sum/(NR-1))**2)}' $1


echo -ne "cen\t"
awk '{sum+=$5; sumsq+=$5*$5} END {print sum/(NR-1) "\t\t" sqrt(sumsq/(NR-1) - (sum/(NR-1))**2)}' $1
