#!/bin/bash
parametr1=$1
parametr2=$2
parametr3=$3
parametr4=$4
parametr5=$5
parametr6=$6
for((i=0;i<parametr1;i++))
do
for((j=1;j<=parametr2;j++))
do
for((k=0;k<parametr3;k++))
do
for((m=k+1;m<=parametr4;m++))
do
for((l=m;l<=parametr5;l++))
do
for((n=1;n<=parametr6;n++))
do
./mealy_rand_gen -s $i --states $j --min $k --max $m --in $l --out $n -f machine$i$j$k$m$l$n.json
wait
echo "machine$i$j$k$m$l$n.json"
./mealy_inp_seq_gen -m states machine$i$j$k$m$l$n.json > input.txt
wait
./mealy_inp_seq_check -m states -i input.txt machine$i$j$k$m$l$n.json
wait
./mealy_inp_seq_gen -m transitions machine$i$j$k$m$l$n.json > input.txt
wait
./mealy_inp_seq_check -m transitions -i input.txt machine$i$j$k$m$l$n.json
wait
./mealy_inp_seq_gen -m paths machine$i$j$k$m$l$n.json > input.txt
wait
./mealy_inp_seq_check -m paths -i input.txt machine$i$j$k$m$l$n.json
done
done
done
done
done
done