cat tmp2.txt | grep -A1 'bx 20 tell me' | grep -v '\-\-' > tmp3.txt
cat tmp3.txt | tr '\n' ' ' | tr '?' '\n' | awk '{print $6, $7, $8}' | grep [a-z] > tmp4.txt
cat tmp4.txt | while read a b c; do
    #echo "the $c is a $b"
    #echo "the $c is $a"
    #grep "the $c is a[n|]* $b" ../../template/skills/categories_list.txt
    #grep "the $c is $a" ../../template/skills/adjectives_list.txt
    g1=$(grep -c "the $c is a[n|]* $b" ../../template/skills/categories_list.txt)
    g2=$(grep -c "the $c is $a" ../../template/skills/adjectives_list.txt)
    g=$(expr $g1 + $g2)
    if [ $g -eq 2 ]; then
	echo ok
    else
	echo no
    fi
done > tmp5.txt
#echo -n "? bx tell me errors: "
#cat tmp5.txt | grep no | wc -l

cat tmp2.txt | grep -A1 '? tell me' | grep -v '\-\-' > tmp6.txt
cat tmp6.txt | tr '\n' ' ' | tr '?' '\n' | awk '{print $4, $5}' | grep [a-z] > tmp7.txt
cat tmp7.txt | while read a b; do
    #echo "the $b is a $a"
    #grep "the $b is a[n|]* $a" ../../template/skills/categories_list.txt
    g=$(grep -c "the $b is a[n|]* $a" ../../template/skills/categories_list.txt)
    if [ $g -eq 1 ]; then
	echo ok
    else
	echo no
    fi
done > tmp8.txt

#echo -n "? tell me errors: "
#cat tmp8.txt | grep no | wc -l

tell_me_good=$(cat tmp5.txt tmp8.txt | grep ok | wc -l)
echo "\"tell\" me good answers: $tell_me_good"
tell_me_err=$(cat tmp5.txt tmp8.txt | grep no | wc -l)
echo "\"tell\" me errors: $tell_me_err"
echo $tell_me_good > tmp9.txt
echo $tell_me_err > tmp10.txt
