for round in $(seq 1 4); do
    ../bin/train_goto $round 8 > train_goto_${round}.txt
done

