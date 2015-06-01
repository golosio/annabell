for round in $(seq 1 4); do (
	echo ".load links_people_body_skills_childes.dat"
	echo ".f ../train_goto_files/train_goto_${round}.txt"
	echo ".q") | annabell
done
mv links* ../links_goto
