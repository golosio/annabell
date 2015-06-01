for i in $(seq 0 27); do (
	echo ".load ../links/links_people_body_skills_childes_bring.dat"
	echo ".logfile log_bring_$i.txt"
	echo ".f test_bring_files/test_bring_$i.txt"
	echo ".q") | annabell
done

