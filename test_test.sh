function log2 {
    local x=0
    for (( y=$1-1 ; $y > 0; y >>= 1 )) ; do
        let x=$x+1
    done
    echo $x
}

function compute_processes {
	numbers=$1
	#small hack for computing processors for small amount of numbers
	if [ $numbers -eq 1 ] || [ $numbers -eq 2 ] ; then
	    proc=1
	#number of processors needs to be equal to 2*log(numbers) - 1
	#and number of leafs processors is 'm', where n = 2^m
	else
	    log=$(log2 $numbers)
		#echo $log
	    proc=$(expr 2 \* $log)
		#echo $proc
	    #proc=$(expr $proc - 1) ?? takze to -1 tam nepotrebujem? 
		#echo $proc
	    log_proc=$(log2 $proc)
		#echo $log_proc
	    pow2=$(echo $((2 ** $log_proc)))
		#echo $pow2
	    proc=$(($pow2 - 1))
		#echo $proc
	fi
	echo $proc
}

for i in {1..2}; do 
	if [ "1" -ne $(compute_processes ${i}) ]; then
		echo "FAIL FOR: ${i}" 
		exit
	fi
done

for i in {3..4}; do 
	if [ "3" -ne $(compute_processes ${i}) ]; then
		echo "FAIL FOR: ${i}" 
		exit
	fi
done

for i in {5..16}; do 
	if [ "7" -ne $(compute_processes ${i}) ]; then
		echo "FAIL FOR: ${i}" 
		exit
	fi
done

for i in {17..256}; do 
	if [ "15" -ne $(compute_processes ${i}) ]; then
		echo "FAIL FOR: ${i}" 
		exit
	fi
done

for i in {257..300}; do 
	if [ "31" -ne $(compute_processes ${i}) ]; then
		echo "FAIL FOR: ${i}" 
		exit
	fi
done

#for ((i=0;i<${#in[@]};++i)); do
#	if [ ${out[i]} -ne $(compute_processes ${in[i]}) ]; then
#		echo "FAIL FOR: ${in[i]}" 
#		exit
#	fi
    #echo ${out[i]}
    #echo $(compute_processes ${in[i]})
#done

echo "PASS"