import argparse
import tqdm
import sh

def main(args):
    binary = sh.Command(args.binary_path)
    maximum = 0
    bar = tqdm.trange(100)
    for i in bar:
        bar.set_description(f"current max {maximum}", refresh=True)
        res = binary().stdout.decode("ascii")
        num_idx = res.rindex("OOC ")
        cur = int(res[num_idx + 4:])
        maximum = max(maximum, cur)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--binary_path", required=True)
    main(parser.parse_args())