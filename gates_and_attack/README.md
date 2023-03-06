Command lines:

Figure 8:
touch /tmp/amplification_100ms && rm /tmp/amplification_100ms && for i in {1..10}; do echo $i; /tmp/main test_amplification >> /tmp/amplification_100ms; done
python3 scripts/unify_multiple_test_amplification_graph.py /mnt/toshiba/tmp/amplification_100ms  /mnt/toshiba/tmp/unified_amplification_100ms
python scripts/test_amplification_graph.py -i /mnt/toshiba/tmp/unified_amplification_100ms


Figure 9:
out/Default/chrome --js-flags="--allow-natives-syntax" http://localhost:8000/ 2>/tmp/wasm_amplification_1ms.txt
I ran this, loaded main.html, and refreshed the page 10 times, after that:
python3 scripts/unify_multiple_test_amplification_graph.py /mnt/toshiba/tmp/wasm_amplification_1ms.txt  /mnt/toshiba/tmp/unified_wasm_amplification_1ms.txt
python scripts/test_amplification_graph.py -i /mnt/toshiba/tmp/unified_wasm_amplification_1ms.txt