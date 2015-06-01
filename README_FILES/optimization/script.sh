echo "wk: max weight of links from WGEqWkPh to ElActfSt"

:>wk.dat
wk=1
cat optimization.txt | grep '^wk[0-9]' | awk '{print $3, $4, $5, $6}' | tr ',' '+' | while read line; do
    good=$(echo "100*( $line )/4" | bc -l)
    printf "wk=%d\t( $line )/4 = %.3f\n" $wk $good
    printf "%d\t%.3f\n" $wk $good >> wk.dat
    wk=$(expr $wk + 1)
done
echo

echo "wg: max weight of links from WGEqGoalWG to ElActfSt"
:>wg.dat
wg=3
cat optimization.txt | grep '^wg[0-9]' | awk '{print $3, $4, $5, $6}' | tr ',' '+' | while read line; do
    good=$(echo "100*( $line )/4" | bc -l)
    printf "wg=%d\t( $line )/4 = %.3f\n" $wg $good
    printf "%d\t%.3f\n" $wg $good >> wg.dat
    wg=$(expr $wg + 1)
done
echo

echo "K: number of K winners for winner-take-all rule"
:>K.dat
K=1
cat optimization.txt | grep '^K[0-9],' | awk '{print $3, $4, $5, $6}' | tr ',' '+' | while read line; do
    good=$(echo "100*( $line )/4" | bc -l)
    printf "K=%d\t( $line )/4 = %.3f\n" $K $good
    printf "%d\t%.3f\n" $K $good >> K.dat
    K=$(expr $K + 1)
done
echo

