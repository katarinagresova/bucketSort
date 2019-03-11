#!/bin/bash
# PRL Project 2 2019 - Bucket sort test script
# Author: Katarina Gresova (xgreso00)
# Mail: xgreso00@stud.fit.vutbr.cz

if [ "$#" -gt 1 ]; then #there are more parameters than 1
    echo "Illegal number of parameters"
    exit 1
elif [ "$#" -eq 0 ]; then #there is none parameter
    echo "Illegal number of parameters"
    exit 1
elif [ $1 -lt 1 ]; then
    echo "Illegal value of numbers to sort"
    exit 1
else
	numbers=$1;
fi

#create random numbers file
dd if=/dev/random bs=1 count=$numbers of=numbers 2>/dev/null

#compile
mpic++ --prefix /usr/local/share/OpenMPI -o bks bks.cpp

#helper function to compute logarithm rounded up
function log2 {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

#small hack for computing processors for small amount of numbers
if [ $numbers -eq 1 ] || [ $numbers -eq 2 ] ; then
    proc=1
#number of processors needs to be equal to 2*log(numbers) - 1
#and number of leafs processors is 'm', where n = 2^m
else
    log=$(log2 $numbers)
    proc=$(expr 2 \* $log)
    log_proc=$(log2 $proc)
    pow2=$(echo $((2 ** $log_proc)))
    proc=$(($pow2 - 1))
fi

#run
mpirun --prefix /usr/local/share/OpenMPI -np $proc bks $numbers

#remove
rm -f bks numbers