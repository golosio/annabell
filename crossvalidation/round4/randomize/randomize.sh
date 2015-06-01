cat ../people/output_all.txt.correct > tmp1.txt
#cat tmp1.txt | grep '^?' | awk '{print NR, $0}' > questions.txt
#cat tmp1.txt | grep -v '^?' | awk '{print NR, $0}' > answers.txt
cat tmp1.txt | grep '^?' > q_people.txt
cat tmp1.txt | grep -v '^?' > a_people.txt

cat ../body/output_all.txt.correct > tmp2.txt
#cat tmp2.txt | grep '^?' | awk '{print NR, $0}' > questions.txt
#cat tmp2.txt | grep -v '^?' | awk '{print NR, $0}' > answers.txt
cat tmp2.txt | grep '^?' > q_body.txt
cat tmp2.txt | grep -v '^?' > a_body.txt

cat ../skills/log2.txt.correct | grep -v '\->' | grep -v '\.' | grep -v '>>' | grep -v animal | sed 's/\-es/\-s/g' > tmp3.txt
cat tmp3.txt | grep -A1 '^?' | grep -v '\-\-' > tmp4.txt
#cat tmp4.txt | grep '^?' | awk '{print NR, $0}' > questions.txt
#cat tmp4.txt | grep -v '^?' | awk '{print NR, $0}' > answers.txt
cat tmp4.txt | grep '^?' | sed 's/? /?2 /' > q_skills2.txt
cat tmp4.txt | grep -v '^?' > a_skills2.txt

cat ../skills/test1.txt | grep '^tell' > q_skills1.txt
nn=$(cat q_skills1.txt | wc -l)
for i in $(seq 1 $nn); do echo xxx; done >  a_skills1.txt

cat ../skills/test3.txt | grep '^\.bx' > q_skills3.txt
nn=$(cat q_skills3.txt | wc -l)
for i in $(seq 1 $nn); do echo xxx; done >  a_skills3.txt

cat q_people.txt q_body.txt q_skills1.txt q_skills2.txt q_skills3.txt  | awk '{print NR, $0}' > questions.txt
cat a_people.txt a_body.txt a_skills1.txt a_skills2.txt a_skills3.txt  | awk '{print NR, $0}' > answers.txt

nn=$(cat questions.txt | wc -l)
rand_seq/rand_seq $nn > tmp_rand.txt
for i in $(cat tmp_rand.txt); do
    cat questions.txt | grep "^$i "
done > questions_rand.txt
for i in $(cat tmp_rand.txt); do
    cat answers.txt | grep "^$i "
done > answers_rand.txt

echo ".logfile log_rand.txt" > test_rand.txt
echo ".time" >> test_rand.txt
echo >> test_rand.txt

cat questions_rand.txt | while read a b line; do
    if [ $b == "tell" ]; then
	echo "$b $line"
	echo ".xr"
    elif [ $b == "?2" ]; then
	echo "? $line"
	echo ".cx"
	echo "? what kind of animal"
	echo ".x"
    elif [ $b == ".bx" ]; then
	echo "$b $line"
    else
	echo "$b $line"
	echo ".cx"
    fi
    echo
done >> test_rand.txt

echo ".time" >> test_rand.txt
echo ".logfile off" >> test_rand.txt
