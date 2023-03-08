import re
import argparse
from itertools import zip_longest
from dataclasses import dataclass

def grouper(iterable, n, fillvalue=None):
    args = [iter(iterable)] * n
    return zip_longest(*args, fillvalue=fillvalue)

def num(s):
    try:
        return int(s)
    except ValueError:
        return float(s)

@dataclass
class InvocationResults:
    as_expected : int
    total : int
    counter : int
    avg_time : float

@dataclass
class GateResults:
    name: str
    latexname: str
    fan_in : int
    fan_out : int
    ram : InvocationResults
    ic : InvocationResults

latex_dict = {
    "classic branch training" : "bt",
    "counter branch training" : "nbt"
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("-a", "--all", action="store_true")
    args = parser.parse_args()
    
    results = {}
    with args.input as f:
        for line in f:
            switch_size = int(re.findall("Switch size ([\d]+)", line)[0])
            it = grouper(f, 3)
            results[switch_size] = []

            for i in range(2):
                intro, ram, ic = next(it)

                name = re.findall("Results for (.*)", intro)[0]
                branch_training, gate_type, fan_in, fan_out = re.findall("(.*) (.*) ([\d]*)to([\d]*)", name)[0]
                fan_in = int(fan_in)
                fan_out = int(fan_out)
                latex_name = f"{latex_dict[branch_training]}\\{gate_type}gate{{{fan_in}}}{{{fan_out}}}"

                ram_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ram)[0]))
                ic_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ic)[0]))
                results[switch_size].append(GateResults(name, latex_name, fan_in, fan_out, ram_invocation, ic_invocation))
    for switch_size in results:
        print('%d & ' % switch_size, end='')
        for i in range(len(results[switch_size])):
            res = results[switch_size][i]
            # 1 & nbt\nandgate{12}{1} & $ 0.000 \%$ & $ 100.000 \%$ && $ 317 $ & $ 128 $ && nbt\notgate{1}{2} & $ 0.000 \%$ & $ 99.990 \%$ && $ 267 $ & $ 108 $ \\
            print('%s & $ %.01f \\%%$  & $ %.01f \\%%$ && $ %.00f $ & $ %.00f $' % (res.latexname, 100 * res.ic.as_expected / res.ic.total, 100 * res.ram.as_expected / res.ram.total, res.ic.avg_time, res.ram.avg_time), end='')
            if i == len(results[switch_size]) - 1:
                print("\\\\")
            else:
                print("&&", end='')
            # print('switch size %d\t%s\t\t%.03f   %.03f   %.00f   %.00f' % (switch_size, res.name, 100 * res.ic.as_expected / res.ic.total, 100 * res.ram.as_expected / res.ram.total, res.ic.avg_time, res.ram.avg_time))
            #print(res.fan_in, res.fan_out)
    #        print('%s & $ %.03f \\%%$  & $ %.03f \\%%$ & $ %.03f \\%%$  & $ %.03f \\%%$  \\\\' % (res.latexname, res.ic.as_expected / res.ic.total, res.ic.counter / (res.fan_out * res.ic.total), res.ram.as_expected / res.ram.total, res.ram.counter / (res.fan_out * res.ram.total)))
            # print('\\hline')
    # print(results)

    # results = filter(lambda x:x.name.endswith("not 1to1"), results)
    # for gate in results:
    #     print(f"{gate.name} : {gate.ram.avg_time}, {gate.ic.avg_time}")
