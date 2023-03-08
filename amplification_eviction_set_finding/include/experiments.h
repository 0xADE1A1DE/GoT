#pragma once

/*
    This file contains the various main functions of our experiments.
*/

typedef int (*experiment_function_func_type)(int argc, char *argv[]);

int test_amplification(int argc, char *argv[]);
int test_fetch(int argc, char *argv[]);
int test_gates(int argc, char *argv[]);
int test_ev(int argc, char *argv[]);

typedef struct {
    const char *experiment_name;
    experiment_function_func_type experiment_func;
    const char *description;
} experiment_descriptor;

static experiment_descriptor experiments[] = {
    { "test_amplification", test_amplification, "Test the tree amplification." },
    { "test_fetch", test_fetch, "Test fetch_address and rdtsc measurement." },
    { "test_gates", test_gates, "Test the various gates." },
    { "test_ev", test_ev, "Test eviction finding code." }
};