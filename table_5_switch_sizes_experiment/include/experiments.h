#pragma once

/*
    This file contains the various main functions of our experiments.
*/

typedef int (*experiment_function_func_type)(int argc, char *argv[]);

int prime_probe_gnupg(int argc, char *argv[]);
int test_amplification(int argc, char *argv[]);
int test_fetch(int argc, char *argv[]);
int test_gates(int argc, char *argv[]);
int test_ev(int argc, char *argv[]);
int test_retention(int argc, char *argv[]);

typedef struct {
    const char *experiment_name;
    experiment_function_func_type experiment_func;
    const char *description;
} experiment_descriptor;

static experiment_descriptor experiments[] = {
    // {"prime_probe_gnupg", prime_probe_gnupg, "Prime probe an old, slightly modifed, gnupg (v1.4.13) known for non-constant time exponentiation."}, // TODO: add attribution?
    { "test_amplification", test_amplification, "Test the tree amplification." },
    { "test_fetch", test_fetch, "Test fetch_address and rdtsc measurement." },
    { "test_gates", test_gates, "Test the various gates." },
    { "test_ev", test_ev, "Test eviction finding code." },
    // {"test_retention", test_retention, "Test schemes of speculative bit retention over time."},
};