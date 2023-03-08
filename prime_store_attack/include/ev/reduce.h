#pragma once
#include <stdint.h>
#include <stdbool.h>
#include "linked_list.h"

bool reduce_eviction_set(node *sentinel, uint32_t length, uintptr_t evictee);