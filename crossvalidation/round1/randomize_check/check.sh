cat ../randomize/log_rand.txt | sed 's/^tell/? tell/; s/^\.bx /? bx /' | grep -v '\->' | grep -v '\.' | grep -v '>>'  | grep -v 'it is a animal' | grep -v '? what kind of animal' | sed 's/\? which/which/' > tmp1.txt
cat tmp1.txt | grep -A1 '^?' | grep -v '\-\-' > tmp2.txt
cat tmp2.txt | grep '^?' > questions.txt
cat tmp2.txt | grep -v '^?' > answers.txt

k=0
good=0

err_people=0
err_body=0
err_skills=0

:> diff.txt
n1=$(cat ../randomize/a_people.txt | wc -l)
n2=$(cat ../randomize/a_people.txt ../randomize/a_body.txt | wc -l)
cat ../randomize/answers_rand.txt | while read nn l; do
    k=$(expr $k + 1)
    l1=$(head -$k answers.txt | tail -1)
    if [ "$l1" != "$l" ]; then
	echo >> diff.txt
	echo $nn >>diff.txt
	echo $l1 >>diff.txt
	echo $l >>diff.txt
	if [ $nn -le $n1 ]; then
	    err_people=$(expr $err_people + 1)
	elif [ $nn -le $n2 ]; then
	    err_body=$(expr $err_body + 1)
	else
	    err_skills=$(expr $err_skills + 1)
	fi
	echo $err_people > err_people.txt
	echo $err_body > err_body.txt
	echo $err_skills > err_skills.txt
    else
	good=$(expr $good + 1)
	echo $good > good.txt
    fi
done


nxxx=$(cat ../randomize/answers_rand.txt | grep xxx | wc -l)
echo "num. of \"tell me\" questions: $nxxx"

#echo The number of good xxx must be subtracted from the skills errors,
#echo and added to the total number of good answers
#echo edit the file ../randomize/log_rand.txt ,
#echo search the questions that start with \"tell me\" 
#echo and check the answer

./check2.sh

q_animal=$(cat ../randomize/log_rand.txt | grep '^? what kind of' | wc -l)
good_animal=$(cat ../randomize/log_rand.txt | grep -B1 '^? what kind of' | grep '^it is a animal$' | wc -l)
err_animal=$(expr $q_animal - $good_animal)
echo "it is an animal errors: $err_animal / $q_animal"


tell_me_good=$(cat tmp9.txt)

err_people=$(cat err_people.txt)
err_body=$(cat err_body.txt)
err_skills=$(cat err_skills.txt)
err_skills=$(expr $err_skills - $tell_me_good)
echo -n "people errors: $err_people / "
cat ../randomize/q_people.txt | wc -l

echo -n "body errors: $err_body / "
cat ../randomize/q_body.txt | wc -l

err_skills=$(expr $err_skills + $err_animal)

q_skills1=$(cat ../randomize/q_skills1.txt ../randomize/q_skills2.txt ../randomize/q_skills3.txt | wc -l)
q_skills=$(expr $q_skills1 + $q_animal)
echo "skills errors: $err_skills / $q_skills"


good=$(cat good.txt)
good=$(expr $good + $tell_me_good + $good_animal)
qtot=$(expr $(cat answers.txt | wc -l) + $q_animal)
echo "good: $good / $qtot"

