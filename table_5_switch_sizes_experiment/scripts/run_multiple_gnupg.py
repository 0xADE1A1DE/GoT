import argparse
import sh

def main(args):
    binary = sh.Command(args.binary_path)

    iterations = 0
    with open(args.result_path, "w") as f:
        while iterations < args.iterations:
            try:
                res = binary("prime_probe_gnupg").stdout.decode("ascii")
                print(iterations)
                f.write(res)
                iterations += 1
            except:
                pass


if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--binary_path", required=True)
    parser.add_argument("-r", "--result_path", required=True)
    parser.add_argument("-i", "--iterations", default=100, type=int)
    main(parser.parse_args())