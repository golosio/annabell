date > start.txt
cd randomize
(echo .load ../links/links_people_body_skills_childes_bring.dat
echo .f test_rand.txt
echo .q) | annabell
#
cd ../childes
(echo .load ../links/links_people_body_skills_childes_bring.dat
echo .f david_test_all.txt
echo .q) | annabell
#
cd ../virtual
./test_bring.sh
#
cd ..
date > end.txt
