for round in $(seq 1 4); do
    for tr_ex in $(seq 1 8); do
	for i in $(seq 0 27); do (
		echo ".load ../links_bring/links_bring_${round}_${tr_ex}.dat"
		echo ".logfile log_bring_${round}_${tr_ex}_$i.txt"
		echo ".f ../test_bring_files/test_bring_$i.txt"
		echo ".q") | annabell
	done
    done
done


