import matplotlib.pyplot as plt
import argparse
import numpy
import math
import re


FIRST_EXP_INDICATOR = "RAM"
SECOND_EXP_INDICATOR = "IC"

XLABEL = "Time in ms"
YLABEL = "amount"

LEGEND_FIRST_EXP = "root out of cache"
LEGEND_SECOND_EXP = "root in cache"

DEFAULT_TITLE = "Graph of 'test_amplification' %s\n %d samples per label"


def main(args):
    with args.input as f:
        first_exp_results, second_exp_results = parse_results(f.read())
        display_results(first_exp_results, second_exp_results, args.extra_title_text)


def display_results(first_exp_results, second_exp_results, extra_title_text):
    title = DEFAULT_TITLE % (extra_title_text, len(first_exp_results))

    number_of_bins = 100
    percentile = 99.9
    xlim_max = max(numpy.percentile(first_exp_results, percentile),
                   numpy.percentile(second_exp_results, percentile)) + 0.1
    bins = numpy.arange(0, xlim_max, xlim_max / number_of_bins)

    plt.title(title)
    plt.xlabel(XLABEL)
    plt.ylabel(YLABEL)
    plt.xlim(0, xlim_max)

    plt.hist(first_exp_results, bins=bins, label=LEGEND_FIRST_EXP, alpha=0.7)
    plt.hist(second_exp_results, bins=bins, label=LEGEND_SECOND_EXP, alpha=0.7)

    plt.legend(loc="upper right")

    plt.show()


def parse_results(contents):
    first_exp_start = contents.index(FIRST_EXP_INDICATOR)
    second_exp_start = contents.index(SECOND_EXP_INDICATOR)
    first_exp = contents[first_exp_start: second_exp_start]
    second_exp = contents[second_exp_start:]

    first_exp_results = parse_exp(first_exp)
    second_exp_results = parse_exp(second_exp)
    return first_exp_results, second_exp_results


def parse_exp(exp):
    total_results = []
    exp_lines = filter(lambda line: "Took" in line, exp.splitlines())
    for exp_line in exp_lines:
        total_results += list(map(float, re.findall("([\d.]+) ms", exp_line)))
    return total_results


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument(
        "-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("-e", "--extra-title-text", default="")
    main(parser.parse_args())
