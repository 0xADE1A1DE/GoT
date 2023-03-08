import argparse
from colorama import Fore, Style

def main(args):
    with args.input as f:
        data = f.read()
        results = data.splitlines()[-1]

        counts = split_to_counts(results)
        counts_groups = split_to_groups(counts)
        for counts in counts_groups:
            try:
                decode_counts(counts)
            except:
                pass

def decode_counts(counts):
    counts = replace_small_counts('0', '1', counts, 4)
    counts = coalesce(counts)
    counts = replace_small_counts('1', '0', counts, 6)
    counts = coalesce(counts)

    if len(counts) < 10:
        return

    for count in counts:
        print(f"{Style.NORMAL + Fore.LIGHTCYAN_EX if count[0] == '1' else Style.NORMAL}{count[1]}", end=f"{Style.RESET_ALL},")
    print("")

    # only intersted in the zero sequences from now on.
    counts = list(filter(lambda count: count[0] == '0', counts))
    key = "".join(['1' if count[1] > 50 else '0' for count in counts])
    print(key, len(key))

def split_to_groups(counts, bar=1000, discard=True):
    output = [[]]
    for i, val in enumerate(counts):
        if val[1] >= bar:
            if i == len(counts) - 1:
                break
            if not discard:
                output.append([val])
            output.append([])
        else:
            output[-1].append(val)
    return output

def split_to_counts(results):
    counts = []
    current_char = None
    current_count = 0
    for result in results:
        if result != current_char:
            if current_char != None:
                counts.append((current_char, current_count))

            current_char, current_count = result, 1
        else:
            current_count += 1
    
    if current_char != None:
        counts.append((current_char, current_count))
    return counts

def replace_small_counts(replacee, replace_with, counts, bar=5):
    updated = []
    for count in counts:
        if count[0] == replacee and count[1] < bar:
            updated.append((replace_with, count[1]))
        else:
            updated.append(count)
    return updated

def coalesce(counts):
    updated = []
    current_char = counts[0][0]
    current_count = 0
    for count in counts:
        if count[0] != current_char:
            updated.append((current_char, current_count))
            current_char, current_count = count
        else:
            current_count += count[1]
    
    updated.append((current_char, current_count))
    return updated

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--input", type=argparse.FileType("r"), required=True)
    main(parser.parse_args())