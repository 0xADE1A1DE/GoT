# Preparation

## Prime+Store

```bash
# building
cd ~/GoT/prime_store_attack/src/
rm -rf ../build
mkdir ../build
make all
rm -rf ~/prime_store
cp -r ../build ~/prime_store
```

# Basic Test

We expect that at `~/prime_store`, there would be a binary `main`

It should output:

```
Usage: /home/usenix/prime_store/main [experiment]

             prime_probe_gnupg          Prime probe an old, slightly modifed, gnupg (v1.4.13) known for non-constant time exponentiation.

            test_amplification          Test the tree amplification.

                    test_gates          Test the various gates.

                test_retention          Test schemes of speculative bit retention over time.
```

# Evaluation workflow

## Prime+Store:

```bash
sudo ~/prime_store/main prime_probe_gnupg > /tmp/prime_store_traces.txt
```

We consider `prime_store_traces.txt` to be the artifact of this experiment, gathering the traces take approximately 10 hours.