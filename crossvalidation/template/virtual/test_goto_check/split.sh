for round in $(seq 1 4); do
    for tr_ex in $(seq 1 8); do
	grep -n '^#' log_goto_${round}_${tr_ex}.txt | sed 's/:.*//' > tmp.txt
	cat log_goto_${round}_${tr_ex}.txt | wc -l >> tmp.txt
	a1=0
	k=0
	cat tmp.txt | while read a; do
	    if [ $a1 != 0 ]; then
		nl=$(expr $a - $a1)
		cat log_goto_${round}_${tr_ex}.txt | tail -n+$a1 | head -$nl \
		    > log_goto_${round}_${tr_ex}_$k.txt
		k=$(expr $k + 1)
	    fi
	    a1=$a
	done
	rm tmp.txt
    done
done
