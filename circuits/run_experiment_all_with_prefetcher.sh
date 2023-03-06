#!/bin/bash

# MAKE SURE THAT ./run_experiment_pipe_auto.sh runs the experiments 7 times (* 10000)

if [ -d $1 ]
then
    echo "Directory exists!"
    exit
fi

mkdir -pv $1


./compile.sh

echo 10000 > REP
echo 13 > DELAY

echo "prefetch" > PREFETCH
mkdir -pv $1/SHA1_PREFETCH

./run_experiment_pipe_auto.sh $1/SHA1_PREFETCH 0 sha1 0 $(cat REP)

mkdir -pv $1/SHA1_PREFETCH/RESULT

cat $1/SHA1_PREFETCH/* | grep "SHA1 Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" > $1/SHA1_PREFETCH/RESULT/all.csv
cat $1/SHA1_PREFETCH/RESULT/all.csv | datamash median 1 > $1/SHA1_PREFETCH/RESULT/ALL_MEDIAN
cat $1/SHA1_PREFETCH/RESULT/all.csv | datamash mean 1 > $1/SHA1_PREFETCH/RESULT/ALL_MEAN

cat $1/SHA1_PREFETCH/* | grep "SHA1 Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" | shuf -n 10000 > $1/SHA1_PREFETCH/RESULT/final.csv
cat $1/SHA1_PREFETCH/RESULT/final.csv | datamash median 1 > $1/SHA1_PREFETCH/RESULT/FINAL_MEDIAN
cat $1/SHA1_PREFETCH/RESULT/final.csv | datamash mean 1 > $1/SHA1_PREFETCH/RESULT/FINAL_MEAN

mkdir -pv $1/ALU

./run_experiment_pipe_auto.sh $1/ALU 0 alu 0 $(cat REP)

mkdir -pv $1/ALU/RESULT

cat $1/ALU/* | grep "ALU Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" > $1/ALU/RESULT/all.csv
cat $1/ALU/RESULT/all.csv | datamash median 1 > $1/ALU/RESULT/ALL_MEDIAN
cat $1/ALU/RESULT/all.csv | datamash mean 1 > $1/ALU/RESULT/ALL_MEAN

cat $1/ALU/* | grep "ALU Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" | shuf -n 10000 > $1/ALU/RESULT/final.csv
cat $1/ALU/RESULT/final.csv | datamash median 1 > $1/ALU/RESULT/FINAL_MEDIAN
cat $1/ALU/RESULT/final.csv | datamash mean 1 > $1/ALU/RESULT/FINAL_MEAN


mkdir -pv $1/ALU_MAJ

./run_experiment_pipe_auto.sh $1/ALU_MAJ 0 alu_maj 0 $(cat REP)

mkdir -pv $1/ALU_MAJ/RESULT

cat $1/ALU_MAJ/* | grep "ALU Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" > $1/ALU_MAJ/RESULT/all.csv
cat $1/ALU_MAJ/RESULT/all.csv | datamash median 1 > $1/ALU_MAJ/RESULT/ALL_MEDIAN
cat $1/ALU_MAJ/RESULT/all.csv | datamash mean 1 > $1/ALU_MAJ/RESULT/ALL_MEAN

cat $1/ALU_MAJ/* | grep "ALU Acc*" | grep "(.*)" -o | sed "s/(//g" | sed "s/)//g" | shuf -n 10000 > $1/ALU_MAJ/RESULT/final.csv
cat $1/ALU_MAJ/RESULT/final.csv | datamash median 1 > $1/ALU_MAJ/RESULT/FINAL_MEDIAN
cat $1/ALU_MAJ/RESULT/final.csv | datamash mean 1 > $1/ALU_MAJ/RESULT/FINAL_MEAN



mkdir -pv $1/GOL_GLIDER_1

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_1 0 gol_glider 1 200

mkdir -pv $1/GOL_GLIDER_1/RESULT

cat $1/GOL_GLIDER_1/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_1/RESULT/all.csv
cat $1/GOL_GLIDER_1/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_1/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_1/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_1/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_1/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_1/RESULT/final.csv
cat $1/GOL_GLIDER_1/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_1/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_1/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_1/RESULT/FINAL_MEAN


mkdir -pv $1/GOL_GLIDER_10

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_10 0 gol_glider 10 200

mkdir -pv $1/GOL_GLIDER_10/RESULT

cat $1/GOL_GLIDER_10/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_10/RESULT/all.csv
cat $1/GOL_GLIDER_10/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_10/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_10/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_10/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_10/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_10/RESULT/final.csv
cat $1/GOL_GLIDER_10/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_10/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_10/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_10/RESULT/FINAL_MEAN


mkdir -pv $1/GOL_GLIDER_20

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_20 0 gol_glider 20 200

mkdir -pv $1/GOL_GLIDER_20/RESULT

cat $1/GOL_GLIDER_20/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_20/RESULT/all.csv
cat $1/GOL_GLIDER_20/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_20/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_20/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_20/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_20/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_20/RESULT/final.csv
cat $1/GOL_GLIDER_20/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_20/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_20/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_20/RESULT/FINAL_MEAN



mkdir -pv $1/GOL_GLIDER_30

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_30 0 gol_glider 30 200

mkdir -pv $1/GOL_GLIDER_30/RESULT

cat $1/GOL_GLIDER_30/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_30/RESULT/all.csv
cat $1/GOL_GLIDER_30/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_30/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_30/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_30/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_30/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_30/RESULT/final.csv
cat $1/GOL_GLIDER_30/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_30/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_30/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_30/RESULT/FINAL_MEAN



mkdir -pv $1/GOL_GLIDER_40

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_40 0 gol_glider 40 200

mkdir -pv $1/GOL_GLIDER_40/RESULT

cat $1/GOL_GLIDER_40/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_40/RESULT/all.csv
cat $1/GOL_GLIDER_40/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_40/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_40/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_40/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_40/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_40/RESULT/final.csv
cat $1/GOL_GLIDER_40/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_40/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_40/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_40/RESULT/FINAL_MEAN


mkdir -pv $1/GOL_GLIDER_50

./run_experiment_pipe_auto.sh $1/GOL_GLIDER_50 0 gol_glider 50 200

mkdir -pv $1/GOL_GLIDER_50/RESULT

cat $1/GOL_GLIDER_50/* | grep "Whole.*" | awk -F":" '{print $3}' > $1/GOL_GLIDER_50/RESULT/all.csv
cat $1/GOL_GLIDER_50/RESULT/all.csv | datamash median 1 > $1/GOL_GLIDER_50/RESULT/ALL_MEDIAN
cat $1/GOL_GLIDER_50/RESULT/all.csv | datamash mean 1 > $1/GOL_GLIDER_50/RESULT/ALL_MEAN

cat $1/GOL_GLIDER_50/* | grep "Whole.*" | awk -F":" '{print $3}' | shuf -n 500 > $1/GOL_GLIDER_50/RESULT/final.csv
cat $1/GOL_GLIDER_50/RESULT/final.csv | datamash median 1 > $1/GOL_GLIDER_50/RESULT/FINAL_MEDIAN
cat $1/GOL_GLIDER_50/RESULT/final.csv | datamash mean 1 > $1/GOL_GLIDER_50/RESULT/FINAL_MEAN
