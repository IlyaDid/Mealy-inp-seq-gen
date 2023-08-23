#!/bin/bash
parametr1=$1
parametr2=$2
parametr3=$3
parametr4=$4
parametr5=$5
parametr6=$6
rand=0
if [ $# == 0 ];
then
echo "bash debug.sh num_seeds num_states min_num_transitions max_num_transitions num_inp_symb num_out_symb"
exit
fi
set -e
for((i=0;i<parametr1;i++))
do
rand=$RANDOM
for((j=1;j<=parametr2;j++))
do
for((k=0;k<=parametr3;k++))
do
for((m=k+1;m<=parametr4;m++))
do
for((l=m;l<=parametr5;l++))
do
for((n=1;n<=parametr6;n++))
do
./mealy_rand_gen -s $rand --states $j --min $k --max $m --in $l --out $n -f machine$rand$j$k$m$l$n.json
echo "machine$rand$j$k$m$l$n.json"
./mealy_inp_seq_gen -m states machine$rand$j$k$m$l$n.json > input.txt
./mealy_inp_seq_check -m states -i input.txt machine$rand$j$k$m$l$n.json
./mealy_inp_seq_gen -m transitions machine$rand$j$k$m$l$n.json > input.txt
./mealy_inp_seq_check -m transitions -i input.txt machine$rand$j$k$m$l$n.json
./mealy_inp_seq_gen -m paths machine$rand$j$k$m$l$n.json > input.txt
./mealy_inp_seq_check -m paths -i input.txt machine$rand$j$k$m$l$n.json
done
done
done
done
done
done