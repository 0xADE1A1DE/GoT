#include <float.h>

#include "gates.h"
#include "consts.h"
#include "util.h"

#include "mm.h"

uintptr_t correct_gate_3(gate_3 gate, uintptr_t a, uintptr_t b, uintptr_t out, uintptr_t trash) {
  uintptr_t aCopies[3];
  uintptr_t bCopies[3];
  uintptr_t results[3];

  aCopies[0] = get_temporary_memory();
  aCopies[1] = get_temporary_memory();
  aCopies[2] = get_temporary_memory();

  bCopies[0] = get_temporary_memory();
  bCopies[1] = get_temporary_memory();
  bCopies[2] = get_temporary_memory();

  results[0] = get_temporary_memory();
  results[1] = get_temporary_memory();
  results[2] = get_temporary_memory();

  mfence();
  lfence();
  clflush(aCopies[0]);
  clflush(aCopies[1]);
  clflush(aCopies[2]);
  clflush(bCopies[0]);
  clflush(bCopies[1]);
  clflush(bCopies[2]);
  clflush(results[0]);
  clflush(results[1]);
  clflush(results[2]);
  mfence();
  lfence();

  trash = fan_gate_3(a, aCopies[0], aCopies[1], aCopies[2], trash);
  mfence();
  lfence();
  trash = fan_gate_3(b, bCopies[0], bCopies[1], bCopies[2], trash);
  mfence();
  lfence();

  trash = gate(aCopies[0], bCopies[0], results[0], trash);
  mfence();
  lfence();
  trash = gate(aCopies[1], bCopies[1], results[1], trash);
  mfence();
  lfence();
  trash = gate(aCopies[2], bCopies[2], results[2], trash);
  mfence();
  lfence();

  trash = majority_gate(results[0], results[1], results[2], out, trash);
  
  mfence();
  lfence();

  return_temporary_memory(results[2]);
  return_temporary_memory(results[1]);
  return_temporary_memory(results[0]);

  return_temporary_memory(bCopies[2]);
  return_temporary_memory(bCopies[1]);
  return_temporary_memory(bCopies[0]);

  return_temporary_memory(aCopies[2]);
  return_temporary_memory(aCopies[1]);
  return_temporary_memory(aCopies[0]);

  return trash;
}

uintptr_t correct_gate_3_5(gate_3 gate, uintptr_t a, uintptr_t b, uintptr_t out, uintptr_t trash) {
  uintptr_t aCopies[5];
  uintptr_t bCopies[5];
  uintptr_t results[5];

  aCopies[0] = get_temporary_memory();
  aCopies[1] = get_temporary_memory();
  aCopies[2] = get_temporary_memory();
  aCopies[3] = get_temporary_memory();
  aCopies[4] = get_temporary_memory();

  bCopies[0] = get_temporary_memory();
  bCopies[1] = get_temporary_memory();
  bCopies[2] = get_temporary_memory();
  bCopies[3] = get_temporary_memory();
  bCopies[4] = get_temporary_memory();

  results[0] = get_temporary_memory();
  results[1] = get_temporary_memory();
  results[2] = get_temporary_memory();
  results[3] = get_temporary_memory();
  results[4] = get_temporary_memory();

  mfence();
  lfence();
  clflush(aCopies[0]);
  clflush(aCopies[1]);
  clflush(aCopies[2]);
  clflush(aCopies[3]);
  clflush(aCopies[4]);
  clflush(bCopies[0]);
  clflush(bCopies[1]);
  clflush(bCopies[2]);
  clflush(bCopies[3]);
  clflush(bCopies[4]);
  clflush(results[0]);
  clflush(results[1]);
  clflush(results[2]);
  clflush(results[3]);
  clflush(results[4]);
  mfence();
  lfence();

  trash = fan_gate_5(a, aCopies[0], aCopies[1], aCopies[2], aCopies[3], aCopies[4], trash);
  mfence();
  lfence();
  trash = fan_gate_5(b, bCopies[0], bCopies[1], bCopies[2], bCopies[3], bCopies[4], trash);
  mfence();
  lfence();

  trash = gate(aCopies[0], bCopies[0], results[0], trash);
  mfence();
  lfence();
  trash = gate(aCopies[1], bCopies[1], results[1], trash);
  mfence();
  lfence();
  trash = gate(aCopies[2], bCopies[2], results[2], trash);
  mfence();
  lfence();
  trash = gate(aCopies[2], bCopies[2], results[3], trash);
  mfence();
  lfence();
  trash = gate(aCopies[2], bCopies[2], results[4], trash);
  mfence();
  lfence();


  trash = majority_gate_5(results[0], results[1], results[2], results[3], results[4], out, trash);

  return_temporary_memory(results[4]);
  return_temporary_memory(results[3]);
  return_temporary_memory(results[2]);
  return_temporary_memory(results[1]);
  return_temporary_memory(results[0]);

  return_temporary_memory(bCopies[4]);
  return_temporary_memory(bCopies[3]);
  return_temporary_memory(bCopies[2]);
  return_temporary_memory(bCopies[1]);
  return_temporary_memory(bCopies[0]);

  return_temporary_memory(aCopies[4]);
  return_temporary_memory(aCopies[3]);
  return_temporary_memory(aCopies[2]);
  return_temporary_memory(aCopies[1]);
  return_temporary_memory(aCopies[0]);

  return trash;
}