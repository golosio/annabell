for round in $(seq 1 4); do
    ../bin/train_bring $round 8 > train_bring_${round}.txt
done

