import re
import argparse
from functools import cmp_to_key
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
    as_expected: int
    total: int
    counter: int
    avg_time: float


@dataclass
class GateResults:
    name: str
    latexname: str
    bt_type: str
    gate_type: str
    fan_in: int
    fan_out: int
    ram: InvocationResults
    ic: InvocationResults


latex_dict = {
    "classic branch training": "bt",
    "counter branch training": "nbt"
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
            bt_type = latex_dict[branch_training]
            if (gate_type == "and" or gate_type == "buffer") and bt_type == "bt":
                bt_type = "fbd"
            if (gate_type == "and" or gate_type == "buffer") and bt_type == "nbt":
                continue
            latex_name = f"{bt_type}\\{gate_type}gate{{{fan_in}}}{{{fan_out}}}"

            ram_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ram)[0]))
            ic_invocation = InvocationResults(*map(num, re.findall(".* we have ([\d]*) / ([\d]*) .* value - ([\d]*).* of ([\d.]*)", ic)[0]))
            results.append(GateResults(name, latex_name, bt_type, gate_type, fan_in, fan_out, ram_invocation, ic_invocation))

    def compare_results(x: GateResults, y: GateResults):
        bt_order = {'bt': 0, 'nbt': 1, 'fbd': 2}
        if bt_order[x.bt_type] - bt_order[y.bt_type] != 0:
            return bt_order[x.bt_type] - bt_order[y.bt_type]
        gate_order = {'nand': 0, 'nor': 1, 'not': 2, 'and': 3, 'buffer': 4}
        if gate_order[x.gate_type] - gate_order[y.gate_type] != 0:
            return gate_order[x.gate_type] - gate_order[y.gate_type]
        if x.fan_out - y.fan_out != 0:
            return x.fan_out - y.fan_out
        return x.fan_in - y.fan_in

    results = sorted(results, key=cmp_to_key(compare_results))
    i = 0
    for res in results:
        if res.fan_in == 3 or res.fan_in == 4 or res.fan_out == 4:
            continue
        if res.fan_out == 8:
            continue
        if res.fan_out == 12:
            continue
        if 'nor' in res.latexname and res.fan_in + res.fan_out > 12:
            continue
        if 'nbt' in res.latexname and res.fan_out > 8:
            continue
        if 'nand' in res.latexname and res.fan_in + res.fan_out > 24:
            continue
        line_end = '\\\\' if (i % 2) == 1 else '&&'
        i += 1
        print('%s & $ %.03f$  & $%.03f $ && $ %.00f $ & $%.00f $ %s' % (res.latexname, 100 * res.ic.as_expected /
              res.ic.total, 100 * res.ram.as_expected / res.ram.total, res.ic.avg_time, res.ram.avg_time, line_end))
