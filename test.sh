#!/bin/bash
# PRL Project 1 2018 - Bucket sort test script
# Author: Katarina Gresova (xgreso00)
# Mail: xgreso00@stud.fit.vutbr.cz

if [ "$#" -gt 1 ]; then #there are more parameters than 1
    echo "Illegal number of parameters"
    exit 1
elif [ "$#" -eq 1 ]; then #there is one parameter
    numbers=$1;
else #there is no parameter
    numbers=5;
fi

#create random numbers file
dd if=/dev/random bs=1 count=$numbers of=numbers 2>/dev/null

#compile
mpic++ --prefix /usr/local/share/OpenMPI -o bks bks.cpp

#number of processors needs to be equal to 2*log(numbers) - 1
#and number of leafs processors is 'm', where n = 2^m
#numbers=$((numbers + 1))
function log2 {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

log=$(log2 $numbers)
proc=$(expr 2 \* $log)
proc=$(expr $proc - 1)

log_proc=$(log2 $proc)
pow2=$(echo $((2 ** $log_proc)))
proc=$(($pow2 - 1))

#run
mpirun --prefix /usr/local/share/OpenMPI -np $proc bks $numbers

#remove
rm -f es numbers
