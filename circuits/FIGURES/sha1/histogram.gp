set term pdf enhanced color size 17,7.2   font "helvetica, 50"

set style fill solid 1.0

set key off

binwidth=0.01
bin(x,width)=width*floor(x/width + 0.5)

set xlabel "Accuracy"
set ylabel "Frequency (normalized)"

stats 'final.csv' using 1 nooutput

#set label at 0.015,0.032  sprintf("Samples: %d\nMean: %f\nMedian: %f", STATS_records, STATS_mean, STATS_median);

offset_from_median_height=0.0

median_height=0.021 + offset_from_median_height

set arrow from bin(STATS_median, binwidth), median_height + 0.0075 to bin(STATS_median, binwidth), median_height lw 10

set label at bin(STATS_median, binwidth) - 0.068, median_height + 0.0075 + 0.002 "Median" font "helvetica, 48"
plot 'final.csv' using (bin($1,binwidth)):(1.0/STATS_records) smooth frequency wit\
h boxes

