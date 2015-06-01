for round in $(seq 1 4); do (
	echo ".load ../../../round$round/links/links_people_body_skills_childes.dat"
	echo ".f bring_howto.txt"
	echo ".f ../train_bring_files/train_bring_${round}.txt"
	echo ".q") | annabell
done
mv links* ../links_bring
