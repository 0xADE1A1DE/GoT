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
    "counter branch training" : "cbt"
}

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("-a", "--all", action="store_true")
    args = parser.parse_args()
    
    results = []
    with args.input as f:
        for l in grouper(f, 3):
            intro, ram, ic = l

            name = re.findall("Results for (.*)", intro)[0]
            branch_training, gate_type, fan_in, fan_out = re.findall("(.*) (.*) ([\d]*)to([\d]*)", name)[0]
            fan_in = int(fan_in)
            fan_out = int(fan_out)
            latex_name = f"{latex_dict[branch_training]}\\{gate_type}gate{{{fan_in}}}{{{fan_out}}}"

            ram_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ram)[0]))
            ic_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ic)[0]))
            results.append(GateResults(name, latex_name, fan_in, fan_out, ram_invocation, ic_invocation))
    for res in results:
# #        if res.fan_in == 11 or res.fan_out == 11:
# #            continue
        if res.fan_in == 4 or res.fan_out == 4:
            continue
        if res.fan_out == 8:
             continue
        if res.fan_out == 12:
            continue
#         if res.fan_out + res.fan_in > 20:
#             continue
#        if not 'dn' in res.latexname:
#            continue
        if 'nor' in res.latexname and res.fan_in + res.fan_out > 12:
            continue
        if 'nbt' in res.latexname and res.fan_out  > 8:
            continue
        if 'nand' in res.latexname and res.fan_in + res.fan_out > 24:
            continue
        print('%s & $ %.03f \\%%$  & $ %.03f \\%%$ & $ %.00f $ & $ %.00f $ \\\\' % (res.latexname, 100 * res.ic.as_expected / res.ic.total, 100 * res.ram.as_expected / res.ram.total, res.ic.avg_time, res.ram.avg_time))
        #print(res.fan_in, res.fan_out)
#        print('%s & $ %.03f \\%%$  & $ %.03f \\%%$ & $ %.03f \\%%$  & $ %.03f \\%%$  \\\\' % (res.latexname, res.ic.as_expected / res.ic.total, res.ic.counter / (res.fan_out * res.ic.total), res.ram.as_expected / res.ram.total, res.ram.counter / (res.fan_out * res.ram.total)))
        print('\\hline')
    print(results)

    # results = filter(lambda x:x.name.endswith("not 1to1"), results)
    # for gate in results:
    #     print(f"{gate.name} : {gate.ram.avg_time}, {gate.ic.avg_time}")
