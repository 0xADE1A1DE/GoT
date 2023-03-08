"""
    This file parses a chrome's console log of `test_ev`, and analyzes the ev's it found.
    Must be run alongside/after chrome (on the same machine), and must be run as root.
"""
import sh
import re
import json
import struct
import argparse
from enum import Enum
from datetime import datetime
import matplotlib.pyplot as plt
from collections import namedtuple
from dataclasses import dataclass, asdict


SLICE_BITS = 3
SET_BITS = 10
ASSOCIATIVITY = 12

@dataclass
class Exp:
    lineno: int
    amount_of_buckets: int
    time: int


def count_bits(n):
    res = 0
    while n:
        n &= (n-1)
        res += 1
    return res


def virt_to_phys(pid, virt):
    with open(f"/proc/{pid}/pagemap", "rb") as f:
        f.seek(virt // 2**12 * 8)
        phys = struct.unpack("<Q", f.read(8))[0]
        phys &= 0x7fffffffffffff
        return (phys << 12) | (virt & (4095))


def phys_to_slice(phys):
    mask = [0x1b5f575440, 0x2eb5faa880, 0x3cccc93100]
    ret = 0
    if SLICE_BITS >= 3:
        ret = (ret << 1) | (count_bits(phys & mask[2]) % 2)
    if SLICE_BITS >= 2:
        ret = (ret << 1) | (count_bits(phys & mask[1]) % 2)
    if SLICE_BITS >= 1:
        ret = (ret << 1) | (count_bits(phys & mask[0]) % 2)
    return ret


def phys_to_cacheset(phys):
    return (phys >> 6) & ((1 << SET_BITS) - 1)

CacheBucket = namedtuple("CacheBucket", ['slice', 'set'])

def virt_to_cache_bucket(pid, virt):
    phys = virt_to_phys(pid, virt)
    return CacheBucket(phys_to_slice(phys), phys_to_cacheset(phys))


def find_pid_and_wasm_heap(wasm_heap_in_mb=400):
    pids = sh.awk(sh.grep(sh.ps("aux", _piped=True), "chrome --type=renderer",  _piped=True), "{print $2}").stdout.decode("ascii")
    for pid in pids.splitlines():
        out = sh.pmap(f"{pid}").stdout.decode("ascii")
        res = re.findall(f"([0-9a-fA-F]+).*{str(wasm_heap_in_mb * 1024)}K", out)
        if len(res) == 1:
            base = int("0x"+res[0], 16)
            return (pid, base)
    assert False, "Couldn't find chrome!"

def sift_experiments(lines):
    start_indicator = None
    experiments = []
    for i, line in enumerate(lines):
        if "Success" in line:
            experiments.append(Exp(i, 0, 0))
        elif re.match(".*Failed [\d]* :\(", line):
            experiments.append(Exp(i, -1, 0))
        elif "Turbofan" in line:
            start_indicator = Exp(i, -1, 0)
    
    timestamps = []
    for exp in [start_indicator] + experiments:
        res = re.findall("/([\d]*[\d]*[\d]*)\.", lines[exp.lineno])
        assert len(res) == 1
        timestamps.append(datetime.strptime(res[0], "%H%M%S"))
    
    for i in range(len(experiments)):
        experiments[i].time = (timestamps[i+1] - timestamps[i]).total_seconds()
    
    return determine_types(experiments, lines)


def determine_types(experiments, lines):
    pid, wasm_heap = find_pid_and_wasm_heap()

    for experiment in experiments:
        if experiment.amount_of_buckets == -1:
            continue

        wasm_offsets = []
        for i in range(experiment.lineno + 1, experiment.lineno + 1 + ASSOCIATIVITY):
            wasm_offsets.append(int(re.findall("0x[0-9a-fA-F]*", lines[i])[0], 16))

        cache_buckets = set()
        for wasm_offset in wasm_offsets:
            cache_buckets.add(virt_to_cache_bucket(pid, wasm_heap + wasm_offset))
        
        experiment.amount_of_buckets = len(cache_buckets)
        # if len(cache_buckets) == 1:
        #     experiment.type = Exptypes.SUCCESS
        # elif len(cache_buckets) <= 3:
        #     experiment.type = Exptypes.SEMI_SUCCESS
        # else:
        #     experiment.type = Exptypes.GARBAGE


    return experiments


def main(args):
    experiments = []
    if args.from_json:
        from_json = json.load(args.input)
        for exp in from_json:
            experiments.append(Exp(**exp))
    else:
        with args.input as f:
            experiments = sift_experiments(f.readlines())

    successes = list(filter(lambda x: x.amount_of_buckets == 1, experiments))
    semi_successes = list(filter(lambda x: 1 < x.amount_of_buckets <= 2, experiments))
    garbage = list(filter(lambda x: x.amount_of_buckets > 2, experiments))
    failures = list(filter(lambda x: x.amount_of_buckets == -1, experiments))

    if args.output:
        for_json = list(map(asdict, experiments))
        json.dump(for_json, args.output)

    print(f"Got {len(successes)} eviction sets, {len(semi_successes)} were close, {len(garbage)} were garbage, {len(failures)} failed.")
    all_timings = list(map(lambda x:x.time, experiments))
    successes_timings = list(map(lambda x:x.time, successes))
    semi_successes_timings = list(map(lambda x:x.time, semi_successes))
    garbage_timings = list(map(lambda x:x.time, garbage))
    failures_timings = list(map(lambda x:x.time, failures))
    fig, ax = plt.subplots()
    ax.set_title("Timings for finding eviction sets in chrome via WASM")
    ax.set_xlabel("seconds")
    ax.set_ylabel("amount")

    # ax.legend(["Successful", "Semi Successful", "Garbage", "Failures"])
    bins=range(0, int(max(all_timings)))
    ax.hist(successes_timings, bins=bins, alpha=0.7, label="Successful")
    ax.hist(semi_successes_timings, bins=bins, alpha=0.7, label="Semi Successful")
    ax.hist(garbage_timings, bins=bins, alpha=0.7, label="Garbage")
    ax.hist(failures_timings, bins=bins, alpha=0.7, label="Failures")
    ax.legend(loc="upper right")

    plt.show()

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("-o", "--output", type=argparse.FileType("w"))
    parser.add_argument("--from-json", action='store_true')
    main(parser.parse_args())