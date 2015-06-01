cd people
(echo .f train_all_rand.txt
echo .save ../links/links_people.dat
echo .q) | annabell
#
cd ../body
(echo .load ../links/links_people.dat
echo .f train_all_rand.txt
echo .save ../links/links_people_body.dat
echo .q) | annabell
#
cd ../skills
(echo .load ../links/links_people_body.dat
echo .f train_all_rand.txt
echo .save ../links/links_people_body_skills.dat
echo .q) | annabell
#
cd ../childes
(echo .load ../links/links_people_body_skills.dat
echo .f david_train_all.txt
echo .save ../links/links_people_body_skills_childes.dat
echo .q) | annabell
#
cd ../virtual
(echo .load ../links/links_people_body_skills_childes.dat
echo .f train_bring.txt
echo .save ../links/links_people_body_skills_childes_bring.dat
echo .q) | annabell
#
cd ..
