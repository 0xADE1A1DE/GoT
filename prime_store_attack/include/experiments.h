#pragma once

/*
    This file contains the various main functions of our experiments.
*/

typedef int (*experiment_function_func_type)(int argc, char *argv[]);

int prime_store_gnupg(int argc, char *argv[]);

typedef struct {
    char *experiment_name;
    experiment_function_func_type experiment_func;
    char *description;
} experiment_descriptor;

static experiment_descriptor experiments[] = {
    {"prime_store_gnupg", prime_store_gnupg, "Prime store an old, slightly modifed, gnupg (v1.4.13) known for non-constant time exponentiation."} // TODO: add attribution?
};