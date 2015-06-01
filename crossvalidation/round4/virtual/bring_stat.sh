:>diff.txt
for i in $(seq 0 27); do    
    cat log_bring_$i.txt | grep -v '^\.q' \
	| grep -v '^\.f' > tmp1.txt
    cat test_bring_files/log_bring_$i.txt.correct \
	| grep -v '^\.q' > tmp2.txt
    diff tmp1.txt tmp2.txt | wc -l >> diff.txt
done
good=$(cat diff.txt | grep 0 | wc -l)
echo -n "$good / 28 = "
echo "$good / 28" | bc -l



