grep -n '^#' log_goto.txt.correct | sed 's/:.*//' > tmp.txt
cat log_goto.txt.correct | wc -l >> tmp.txt
a1=0
k=0
cat tmp.txt | while read a; do
    if [ $a1 != 0 ]; then
	nl=$(expr $a - $a1)
	cat log_goto.txt.correct | tail -n+$a1 | head -$nl \
	> log_goto_$k.txt.correct
	k=$(expr $k + 1)
    fi
    a1=$a
done
rm tmp.txt
