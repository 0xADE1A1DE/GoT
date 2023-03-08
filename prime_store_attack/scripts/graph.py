import matplotlib.pyplot as plt
import argparse
import numpy
import math
import re

FIRST_EXP_INDICATOR = "Testing clflush evictee"
SECOND_EXP_INDICATOR = "Reference - flush with spec"
LAST_EXP_INDICATOR = "Reference - flush without spec"

# THRESHOLD = 0.06
THRESHOLD = 0.00015
HISTOGRAM_XLABEL = "Time in ms"
BAR_XLABEL = "bucket number"
YLABEL = "amount"

LEGEND_FIRST_EXP = "root out of cache"
LEGEND_SECOND_EXP = "root in cache"

ALL_SAMPLES_TITLE_TEMPLATE = "Timings of sub tree amplification and last layers accesses\n %d samples per label"
AGGREGATE_TITLE_TEMPLATE = "Timings of consecutive %d sub tree amplification and last layers accesses\n %d samples per label"

def main(args):
    with args.input as f:
        first_exp_results, second_exp_results = parse_results(f.read(), args.aggregate, args.above_threshold)
        # Temporary debug hack, should be removed.
        if len(first_exp_results) == 0:
            first_exp_results.append(0)
        if len(second_exp_results) == 0:
            second_exp_results.append(0)
        display_results(first_exp_results, second_exp_results, args.aggregate, args.above_threshold, args.title)

def display_results(first_exp_results, second_exp_results, aggregate, above_threshold, alt_title):
    title_template = ""
    if not aggregate:
        title_template = ALL_SAMPLES_TITLE_TEMPLATE
    else:
        title_template = AGGREGATE_TITLE_TEMPLATE

    if above_threshold:
        bins = numpy.arange(len(first_exp_results))
        width = 0.35
        plt.bar(bins - width / 2, first_exp_results, width, label=LEGEND_FIRST_EXP)
        plt.bar(bins + width / 2, second_exp_results, width, label=LEGEND_SECOND_EXP)
        plt.xlabel(BAR_XLABEL)
        plt.ylabel(YLABEL)
        plt.legend(loc="upper right")
        if alt_title:
            plt.title(alt_title)
    else:
        number_of_bins = 100
        percentile = 99.9
        xlim_max = max(numpy.percentile(first_exp_results, percentile), numpy.percentile(second_exp_results, percentile))
        bins = numpy.arange(0, xlim_max, xlim_max / number_of_bins)

        if not aggregate:
            title = title_template % len(first_exp_results)
        else:
            title = title_template % (512, len(first_exp_results))
        if alt_title:
            plt.title(alt_title)
        else:
            plt.title(title)
        plt.xlabel(HISTOGRAM_XLABEL)
        plt.ylabel(YLABEL)
        plt.xlim(0, xlim_max)

        plt.hist(first_exp_results, bins=bins, label=LEGEND_FIRST_EXP, alpha=0.7)
        plt.hist(second_exp_results, bins=bins, label=LEGEND_SECOND_EXP, alpha=0.7)

        plt.legend(loc="upper right")
        
    plt.show()


def parse_results(contents, aggregate, above_threshold, do_transpose=True):
    first_exp_start = contents.index("RAM")
    second_exp_start = contents.index("IC")
    first_exp = contents[first_exp_start: second_exp_start]
    second_exp = contents[second_exp_start:]

    first_exp_results = parse_exp(first_exp, aggregate, above_threshold, do_transpose)
    second_exp_results = parse_exp(second_exp, aggregate, above_threshold, do_transpose)
    return first_exp_results, second_exp_results

def parse_exp(exp, aggregate, above_threshold, do_transpose=True):
    total_results = []
    exp_lines = filter(lambda x: x.startswith("Took"), exp.splitlines())
    for exp_line in exp_lines:
        if above_threshold:
            total_results += [list(map(float, re.findall("([\d.]+) ms", exp_line)))]
        elif aggregate:
            total_results += [sum(map(float, re.findall("([\d.]+) ms", exp_line)))]
        else:
            total_results += list(map(float, re.findall("([\d.]+) ms", exp_line)))
    
    if above_threshold and do_transpose:
        total_results = numpy.transpose(total_results)
        # print(total_results)
        new_results = []
        for line in total_results:
            new_results += [len(list(filter(lambda x: THRESHOLD < x, line)))]
        print(new_results)
        return new_results
    return total_results

def calculate_bins(combined):
    return int(math.sqrt(len(combined)))

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("--aggregate", action="store_true")
    parser.add_argument("--above_threshold", action="store_true")
    parser.add_argument("-t", "--title")
    main(parser.parse_args())