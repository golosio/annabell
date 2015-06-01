for tr_ex in $(seq 1 8); do
    for round in $(seq 1 4); do
	:>diff.txt
	for i in $(seq 0 27); do

	    cat log_bring_${round}_${tr_ex}_$i.txt | grep -v '^\.q' \
		| grep -v '^\.f' > tmp1.txt
	    cat ../test_bring_files/log_bring_$i.txt.correct \
		| grep -v '^\.q' > tmp2.txt
	    diff tmp1.txt tmp2.txt | wc -l >> diff.txt
	done
	echo -n "$round $tr_ex "
	good=$(cat diff.txt | grep 0 | wc -l)
	echo -n "$good / 28 = "
	echo "$good / 28" | bc -l
    done
done


