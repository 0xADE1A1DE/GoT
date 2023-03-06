set term pdf enhanced color size 17,7.2   font "helvetica, 50"

set style fill transparent solid 0.8

set key left

#set ytics 0,0.1,0.4

#set yrange [0:0.8]

#set tmargin at screen 0.35

binwidth=0.01
bin(x,width)=width*floor(x/width + 0.5)

set xlabel "Accuracy"
set ylabel "Frequency (normalized)"

stats 'alu_final_prefetch_disabled.csv' using 1 nooutput name "ALU_NOPREF"
stats 'alu_final_prefetch_enabled.csv' using 1 nooutput name "ALU_PREF"

stats 'alu_majority_final_prefetch_disabled.csv' using 1 nooutput name "ALU_MAJORITY_NOPREF"
stats 'alu_majority_final_prefetch_enabled.csv' using 1 nooutput name "ALU_MAJORITY_PREF"

#set label at 0.015,0.065  sprintf("Samples: %d\nMean: %f\nMedian: %f", STATS_records, STATS_mean, STATS_median);

offset_from_median_height=0.0

median_height=0.51 + offset_from_median_height

get_median_height(median, filename)=system(sprintf("awk 'BEGIN{in_bin=0}{if($1>=%f && $1<%f+%f) in_bin++;}END{print in_bin / NR;}' %s", median, median, binwidth, filename))

ALU_NOPREF_median_height = get_median_height(ALU_NOPREF_median, "alu_final_prefetch_disabled.csv")
print ALU_NOPREF_median_height
set arrow from bin(ALU_NOPREF_median, binwidth), ALU_NOPREF_median_height + 0.18 to bin(ALU_NOPREF_median, binwidth), ALU_NOPREF_median_height lw 10
set label at bin(ALU_NOPREF_median, binwidth) - 0.185, ALU_NOPREF_median_height + 0.18 + 0.008 "Median (PD)" font "helvetica, 48"


ALU_MAJORITY_median_height = get_median_height(ALU_MAJORITY_NOPREF_median, "alu_majority_final_prefetch_disabled.csv") - 0.01
print ALU_MAJORITY_median_height
set arrow from bin(ALU_MAJORITY_NOPREF_median, binwidth), ALU_MAJORITY_median_height + 0.05 to bin(ALU_MAJORITY_NOPREF_median, binwidth), ALU_MAJORITY_median_height lw 10

set label at bin(ALU_MAJORITY_NOPREF_median, binwidth) - 0.205, ALU_MAJORITY_median_height + 0.05 + 0.005 "Median (MPD)" font "helvetica, 48"

normal_median_height=0.05

ALU_PREF_median_height = get_median_height(ALU_PREF_median, "alu_final_prefetch_enabled.csv")
print ALU_PREF_median_height
set arrow from bin(ALU_PREF_median, binwidth), ALU_PREF_median_height + 0.05 to bin(ALU_PREF_median, binwidth), ALU_PREF_median_height lw 10
set label at bin(ALU_PREF_median, binwidth) - 0.105, ALU_PREF_median_height + 0.05 + 0.02 "Median (PE)" font "helvetica, 48"

ALU_MAJORITY_PREF_median_height = get_median_height(ALU_MAJORITY_PREF_median, "alu_majority_final_prefetch_enabled.csv")
print ALU_MAJORITY_PREF_median_height
set arrow from bin(ALU_MAJORITY_PREF_median, binwidth), ALU_MAJORITY_PREF_median_height + 0.11 to bin(ALU_MAJORITY_PREF_median, binwidth), ALU_MAJORITY_PREF_median_height lw 10
set label at bin(ALU_MAJORITY_PREF_median, binwidth) - 0.006, ALU_MAJORITY_PREF_median_height + 0.11 + 0.03 "(MPE)" font "helvetica, 48"


plot 'alu_final_prefetch_disabled.csv' using (bin($1,binwidth)):(1.0/ALU_NOPREF_records) smooth frequency wit\
h boxes title "ALU Prefetcher Disabled" linecolor rgb "#E63462",\
'alu_majority_final_prefetch_disabled.csv' using (bin($1,binwidth)):(1.0/ALU_MAJORITY_NOPREF_records) smooth frequency wit\
h boxes title "ALU Majority Prefetcher Disabled",\
'alu_final_prefetch_enabled.csv' using (bin($1,binwidth)):(1.0/ALU_PREF_records) smooth frequency wit\
h boxes title "ALU Prefetcher Enabled" linecolor rgb "#2667ff",\
'alu_majority_final_prefetch_enabled.csv' using (bin($1,binwidth)):(1.0/ALU_MAJORITY_PREF_records) smooth frequency wit\
h boxes title "ALU Majority Prefetcher Enabled"


