for round in $(seq 1 4); do
    for tr_ex in $(seq 1 8); do (
	    echo ".load ../links_goto/links_goto_${round}_${tr_ex}.dat"
	    echo ".logfile log_goto_${round}_${tr_ex}.txt"
	    echo ".f ../test_goto_files/goto_list.txt"
	    echo ".f ../test_goto_files/test_goto.txt"
	    echo ".q") | annabell
    done
done
