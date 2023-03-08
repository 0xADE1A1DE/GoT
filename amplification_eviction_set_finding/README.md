# Reproduction Instructions

## Setup

Ensure the tests are running on a laptop with i5-8250U, if not your milage may vary.

### Performance mode

To setup running in performance mode (meaning the processor is running exclusively on one frequency), running as superuser:

```bash
for i in {0..7}; do echo performance > /sys/devices/system/cpu/cpu$i/cpufreq/scaling_governor; done
```

As a means to verify the above command has previously ran, one can run:

```bash
cat /sys/devices/system/cpu/cpu0/cpufreq/scaling_governor
```

### Chromium

The code for testing amplification schemes currently assumes access to microarchitectural opcodes, particularly `clflush`. It uses to evict the 'root memory line' out of cache.

Ensure chromium is compiled with the supplied `v8.diff`.

## Table 2: Accuracy of gates on Intel Core(TM) i5-8250U

Compile `for_publication_cpp` branch at commit `d19206bf40d1cc50d0a626b2aa31de66c1fcf005`

```bash
# laptop
/tmp/main test_gates > /tmp/test_gates_result
```

The script `parse_test_gates` converts the output to the latex needed by the paper.

```bash
#host machine
python scripts/parse_test_gates.py -i /mnt/toshiba/tmp/test_gates_result
```

## Table 5: Accuracy and run time for `nbt nand 12to1` and `nbt not 1to2` gates with different number of cases on Intel Core(TM) i5-8250U

Compile `switch_sizes` at commit `ee3bfd86a353e218f0a8ca4eb352b19cf1c8ef1a`

```bash
# To compile, host machine
for i in {1..16} do SWITCH_SIZE=$i make out

# Run 16 binaries, laptop
for i in {1..16}; do echo "Switch size $i" >> /tmp/res.txt; /tmp/main$i test_gates >> /tmp/res.txt;  done

# Parse to latex, host machine
python scripts/parse_switch_test_gates.py -i /mnt/toshiba/tmp/res.txt
```

## Figure 8: Amplification Hyper-Tree in Native

Compile `for_publication_cpp` branch at commit `b0e02df4c56e380ed0d27c82d214360a7dd83d6c` 

```bash
# laptop
touch /tmp/amplification_100ms && rm /tmp/amplification_100ms
# Aggregate 10 runs
for i in {1..10}; do echo $i; /tmp/main test_amplification >> /tmp/amplification_100ms; done
# Unify the outputs, host machine
python3 scripts/unify_multiple_test_amplification_graph.py /mnt/toshiba/tmp/wasm_amplification_1ms.txt  /mnt/toshiba/tmp/unified_wasm_amplification_1ms.txt

python scripts/test_amplification_graph.py -i /mnt/toshiba/tmp/unified_amplification_100ms
```

## Figure 9: Amplification Hyper-Tree in WebAssembly.

Compile `for_publication_cpp` branch at commit `b0e02df4c56e380ed0d27c82d214360a7dd83d6c` 

```bash
# laptop, this is a path to the custom chromium.
out/Default/chrome --js-flags="--allow-natives-syntax" http://localhost:8000/ 2>/tmp/wasm_amplification_1ms.txt
# I ran this, loaded main.html, and refreshed the page 10 times, after that:
# host machine:
python3 scripts/unify_multiple_test_amplification_graph.py /mnt/toshiba/tmp/wasm_amplification_1ms.txt  /mnt/toshiba/tmp/unified_wasm_amplification_1ms.txt
python scripts/test_amplification_graph.py -i /mnt/toshiba/tmp/unified_wasm_amplification_1ms.txt
```

## Figure 10: Time to find an eviction set in Chrome using 0.1 millisecond low-resolution timer

Compile `for_publication_cpp` branch at commit `b7f7268b0ea08d52ac817fc881cb5ba28c0086d1`

```bash
# host machine
CFLAGS="-DCHROME" make wasm && scp -r ../build/main* acrypto@10.0.0.20:/tmp/
# laptop
CHROME_LOG_FILE=/tmp/wasm_ev_chrome.txt google-chrome --enable-logging --v=0 http://localhost:8000/
python3 test_ev.py -i wasm_ev_chrome.txt -o wasm_ev_chrome.json # ALONGSIDE chrome, do not close it!
```

## Unaccounted results:

Some experiments were not done by me, these are:

* Table 1: Game of Life Glider Accuracy
* Table 3: Accuracy of gates on AMD Ryzen 5 3500U
* Table 4: Accuracy of gates on Apple M1 and Samsung Exynos 2100
* Figure 3: ALU Accuracy.
* Figure 4: SHA-1 Accuracy
* Figure 9: A segment of samples of the square operation in modular exponentiation. The bottom show samples in which we detect eviction. The top shows the sample density. Peaks with density above 15 correspond to a square operation.
* Figure 10: Distribution of Stitched Key in Relation to Ground Truth Location
* Figure 11: T-tetromino Heatmap (calculated from 300 repetitions, the brighter the cell the higher accuracy)
* Figure 12: One Generation Game of Life Accuracy
* Claims about T-test analysis (relating to Figures 8 and 9)
