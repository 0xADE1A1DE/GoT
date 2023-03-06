import argparse
import re

def main(args):
    with args.input as f:
        print(f.readline(), end='')
        print(f.readline(), end='')
        buckets = {}
        while True:
            line = f.readline()
            if "problematic" in line:
                break
            bucket = re.findall("\((.*)\)", line)[0]
            buckets[bucket] = buckets.get(bucket, 0) + 1
        
        problematic = {}
        while True:
            line = f.readline()
            if line == "":
                break
            bucket = re.findall("\((.*)\)", line)[0]
            problematic[bucket] = problematic.get(bucket, 0) + 1
        
        to_print = buckets if args.all else problematic
        for bucket in to_print:
            print(f"Lost {problematic.get(bucket, 0)}/{buckets[bucket]} in bucket ({bucket})")
        print(f"Max in a bucket {max(buckets.values())}")
        print(f"Average is {avg(buckets.values())}")

def avg(samples):
    res = 0.0
    for sample in samples:
        res += sample
    return res / len(samples)

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    parser.add_argument("-a", "--all", action="store_true")
    main(parser.parse_args())