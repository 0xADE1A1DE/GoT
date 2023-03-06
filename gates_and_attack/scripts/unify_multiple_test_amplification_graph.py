import argparse

FIRST_EXP_INDICATOR = "RAM"
SECOND_EXP_INDICATOR = "IC"

def main(args):
    results = {FIRST_EXP_INDICATOR : [], SECOND_EXP_INDICATOR : []}
    for input in args.inputs:
        last_seen_indicator = None
        with input as f:
            for line in f:
                if FIRST_EXP_INDICATOR in line:
                    last_seen_indicator = FIRST_EXP_INDICATOR
                elif SECOND_EXP_INDICATOR in line:
                    last_seen_indicator = SECOND_EXP_INDICATOR
                elif "Took" in line:
                    if last_seen_indicator:
                        results[last_seen_indicator].append(line.strip())
    
    with args.output as f:
        print("Root in RAM", file=f)
        for line in results[FIRST_EXP_INDICATOR]:
            print(line, file=f)
        print("Root IC", file=f)
        for line in results[SECOND_EXP_INDICATOR]:
            print(line, file=f)


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("inputs", type=argparse.FileType("r"), nargs="+")
    parser.add_argument("output", type=argparse.FileType("w"))
    main(parser.parse_args())