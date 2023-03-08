from collections import OrderedDict
import matplotlib.pyplot as plt
import argparse
import tqdm
import sh
import re

def main(args):
    shrink_factor = 3
    binary = sh.Command(args.binary_path)
    page_amounts = range(48, 400, 36)
    box_plot_dict = OrderedDict()
    for it in tqdm.tqdm(range(args.iterations)):
        for page in tqdm.tqdm(page_amounts, leave=False):
            for per_ev in tqdm.tqdm(range(1,5), leave=False):
                if page / per_ev > 100:
                    continue
                key = f"{page * 7 / shrink_factor}, {per_ev}"

                if key not in box_plot_dict:
                    box_plot_dict[key] = []

                with open(f"/tmp/retention_results/{page}_{per_ev}.txt", "a") as f:
                    res = binary("test_retention", "-p", str(page), "-e", str(per_ev)).stdout.decode("ascii")
                    decays, pattern = res.splitlines()
                    f.write(f"Iteration {it}:\n{res}\n")
                    numbers = re.findall("([\d]+)/([\d]+)", decays)
                    decayed = int(numbers[0][0])
                    total = int(numbers[0][1])
                    box_plot_dict[key].append(1 - decayed / total)
    
    fig, ax = plt.subplots()
    ax.set_title(f"Retention with tree amplification set to 100us, {args.iterations} samples per bin. shrink_factor of {shrink_factor}")
    ax.set_xlabel("total retention entries, per_ev")
    ax.set_ylabel("precent kept in cache")
    ax.set_ylim(bottom=0.0, top=1.0)
    ax.boxplot(list(box_plot_dict.values()), labels=list(box_plot_dict.keys()))
    plt.show()



if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--binary_path", required=True)
    parser.add_argument("-i", "--iterations", default=100, type=int)
    main(parser.parse_args())