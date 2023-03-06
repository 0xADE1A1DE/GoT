#!/bin/bash

num=$2

if [ -z $num ]
   then
       num=0
       fi


for num in $(seq $num 7)
do
  taskset -c 1 ./out $3 $4 $5 $(cat DELAY) $(cat PREFETCH) > $1/$num
  #echo $num
done
