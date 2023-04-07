# Preparation

## Figures 8 and 9:

```bash
# building
cd ~/GoT/amplification_eviction_set_finding/src/
rm -rf ../build
mkdir ../build
make all
rm -rf ~/figure_8_9
cp -r ../build ~/figure_8_9

# Chrome had an issue with loading wasm from a local file IIRC.
python3 -m http.server --directory ~/figure_8_9/ 8089 &
```

## Figure 10:

```bash
# building
cd ~/GoT/amplification_eviction_set_finding/src/
rm -rf ../build
mkdir ../build
CFLAGS="-DCHROME" make wasm
rm -rf ~/figure_10
cp -r ../build ~/figure_10

# Chrome had an issue with loading wasm from a local file IIRC.
python3 -m http.server --directory ~/figure_10/ 8010 &
```

# Basic Test

We expect that at `~/figure_8_9`, there would be a binary `main`

It should output:

```
Usage: /home/usenix/figure_8_9/main [experiment]

            test_amplification          Test the tree amplification.

                    test_fetch          Test fetch_address and rdtsc measurement.

                    test_gates          Test the various gates.

                       test_ev          Test eviction finding code.
```

Furthermore, using `google-chrome` or the patched `chromium` verify `http://localhost:8089/` and `http://localhost:8010/` provide a directory listing.

Entering `main.html` shall start the experiment.

Note that Figure 9's experiment requires access to our native JS function, and would not work on `google-chrome`

# Evaluation workflow

## Figure 8:

```bash
# Create a raw output file.
touch /tmp/amplification_100ms.txt && rm /tmp/amplification_100ms.txt

# Aggregate 10 runs
for i in {1..10}; do echo $i; ~/figure_8_9/main test_amplification >> /tmp/amplification_100ms.txt; done

# Unify the outputs
python3 ~/GoT/amplification_eviction_set_finding/scripts/unify_multiple_test_amplification_graph.py /tmp/amplification_100ms.txt /tmp/unified_amplification_100ms.txt
```

The resulting file `unified_amplification_100ms.txt` is the artifact of this experiment. It can be drawn into a graph by:

```bash
python3 ~/GoT/amplification_eviction_set_finding/scripts/test_amplification_graph.py -i /tmp/unified_amplification_100ms.txt
```

## Figure 9:

Run chromium and surf into `http://localhost:8089/main.html`.

Refresh the page 10 times (important: let the run finish, visual cues are non trivial).

```bash
/home/acrypto/Documents/daniel/out/Default/chrome --js-flags="--allow-natives-syntax" http://localhost:8089/main.html 2>/tmp/wasm_amplification_1ms.txt
```

Note: When WASM code is running, Emscripten attempt to display a logo after a few seconds. This experiment is relatively fast, it may not show up. Indeed once the code is finished the page is responsive and the logo is no longer visible. Each run usually takes 5-10 seconds, so to be extra safe 20 seconds should suffice.

![image-20230330100015062](https://i.imgur.com/PQxVqYz.png)

Once done, you should have:

* 1 line of `127.0.0.1 - - [timestamp] "GET /main.html HTTP/1.1" 200 -` visible in your terminal.

* 9 lines of `127.0.0.1 - - [timestamp] "GET /main.html HTTP/1.1" 304 -` visible in your terminal.

Next unify the outputs.

```bash
python3 ~/GoT/amplification_eviction_set_finding/scripts/unify_multiple_test_amplification_graph.py /tmp/wasm_amplification_1ms.txt /tmp/unified_wasm_amplification_1ms.txt
```

The resulting file `unified_wasm_amplification_1ms.txt` is the artifact of this experiment. It can be drawn into a graph by:

```bash
python3 ~/GoT/amplification_eviction_set_finding/scripts/test_amplification_graph.py -i /tmp/unified_wasm_amplification_1ms.txt
```

# Figure 10:

Important: In Chrome, we do not have a virtual address to physical address primitive, and we cannot directly ascertain from within the browser if we indeed found an eviction set. As such, it is crucial to run `test_ev.py` **alongside** `google-chrome`, that is while chrome is still running.

```bash
# To start running.
CHROME_LOG_FILE=/tmp/wasm_ev_chrome.txt google-chrome --enable-logging --v=0 http://localhost:8010/main.html &
```

Contrary to the previous experiment, visual cue that the experiment is over is easy, as here the experiment is long enough for the logo to show up. Indeed when the logo is no longer visible. Also, after some time chrome may warn the page is unresponsive, you can safely ignore this.

```bash
# Chrome may move the WASM heap after some time of inactivity, thus invalidating the offsets printed out during the run.
# This script should be ran close to the finish of the experiment, and while google-chrome is still running!
sudo python3 ~/GoT/amplification_eviction_set_finding/scripts/test_ev.py -i /tmp/wasm_ev_chrome.txt -o /tmp/wasm_ev_chrome.json
sudo chmod 777 /tmp/wasm_ev_chrome.json
```

This script also draws the Figure.

We consider `wasm_ev_chrome.json` to be the artifact, the figure can be drawn in a standalone fashion (without chrome running alongside) via:

```bash
python3 ~/GoT/amplification_eviction_set_finding/scripts/test_ev.py -i /tmp/wasm_ev_chrome.json --from-json
```
