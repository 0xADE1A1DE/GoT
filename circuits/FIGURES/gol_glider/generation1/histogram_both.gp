set term pdf enhanced color size 17,7.2   font "helvetica, 50"

set style fill transparent solid 0.8

set key left

binwidth=0.01
bin(x,width)=width*floor(x/width + 0.5)

set xlabel "Accuracy"
set ylabel "Frequency (normalized)"

stats 'final_prefetch_disabled.csv' using 1 nooutput name "NOPREF"
stats 'final_prefetch_enabled.csv' using 1 nooutput name "NORMAL"

#set label at 0.015,0.065  sprintf("Samples: %d\nMean: %f\nMedian: %f", STATS_records, STATS_mean, STATS_median);

offset_from_median_height=0.0

get_median_height(median, filename)=system(sprintf("awk 'BEGIN{in_bin=0}{if($1>=%f && $1<%f+%f) in_bin++;}END{print in_bin / NR;}' %s", median, median, binwidth, filename)) + 0.005

#median_height=0.03 + offset_from_median_height
median_height = get_median_height(NOPREF_median, "final_prefetch_disabled.csv")

set arrow from bin(NOPREF_median, binwidth), median_height + 0.06 to bin(NOPREF_median, binwidth), median_height lw 10

set label at bin(NOPREF_median, binwidth) - 0.12, median_height + 0.0545  + 0.015 "Median (PD)" font "helvetica, 48"

#normal_median_height=0.0285
normal_median_height = get_median_height(NORMAL_median, "final_prefetch_enabled.csv")

set arrow from bin(NORMAL_median, binwidth), normal_median_height + 0.03 to bin(NORMAL_median, binwidth), normal_median_height lw 10

set label at bin(NORMAL_median, binwidth) - 0.0175, normal_median_height + 0.025 + 0.0125 "Median (PE)" font "helvetica, 48"

plot 'final_prefetch_disabled.csv' using (bin($1,binwidth)):(1.0/NOPREF_records) smooth frequency wit\
h boxes title "Prefetcher Disabled" linecolor rgb "#E63462",\
'final_prefetch_enabled.csv' using (bin($1,binwidth)):(1.0/NORMAL_records) smooth frequency wit\
h boxes title "Prefetcher Enabled" linecolor rgb "#2667ff"


