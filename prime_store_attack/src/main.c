#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "experiments.h"

int main(int argc, char *argv[]) {
    if (argc >= 2) {
        for (int i = 0; i < sizeof(experiments) / sizeof(experiment_descriptor); i++) {
            experiment_descriptor current = experiments[i];
            if (strcmp(current.experiment_name, argv[1]) == 0) {
                return current.experiment_func(argc, argv);
            }
        }
    }
    printf("Usage: %s [experiment]\n\n", argv[0]);
    // TODO: print the descriptions in the same alignment.
    for (int i = 0; i < sizeof(experiments) / sizeof(experiment_descriptor); i++) {
        experiment_descriptor current = experiments[i];
        printf("%*s\t\t%s\n\n", 30, current.experiment_name, current.description);
    }
    return 0;
}