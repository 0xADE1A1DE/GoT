#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#include "trace.h"


#define MINSIZE 16

extern inline void trace_set(trace_t trace, int index, int data);
extern inline int trace_get(trace_t trace, int index);
extern inline int trace_len(trace_t trace);



void trace_resize(trace_t trace, int newSize) {
  if (newSize < MINSIZE)
    newSize = MINSIZE;
  if (newSize < trace->size)
    return;
  int *newdata = realloc(trace->data, newSize * sizeof(traceint));
  if (newdata == NULL) {
    fprintf(stderr, "trace_resize: realloc failed\n");
    exit(1);
  }

  memset(newdata+trace->size, 0, (newSize-trace->size) * sizeof(traceint));
  trace->size = newSize;
  trace->data = newdata;
}


trace_t trace_new(int size) {
  trace_t rv = malloc(sizeof(struct trace));
  if (rv == NULL) {
    fprintf(stderr, "trace_new: alloc failed\n");
    exit(1);
  }
  rv->len = rv->size = 0;
  rv->data = NULL;
  rv->flags = 0;
  trace_resize(rv, size);
  return rv;
}


void trace_free(trace_t trace) {
  free(trace->data);
  free(trace);
}

static void trace_autofree(trace_t trace) {
  // Later store it somewhere for protection
  if (trace->flags & TF_AUTOFREE) 
    trace_free(trace);
}


void trace_setautofree(trace_t trace, int autofree) {
  if (autofree)
    trace->flags |= TF_AUTOFREE;
  else
    trace->flags &= ~TF_AUTOFREE;
}

int trace_getautofree(trace_t trace) {
  return (trace->flags & TF_AUTOFREE) != 0;
}


void trace_clear(trace_t trace) {
  if (trace->data == NULL)
    return;
  memset(trace->data, 0, trace->size * sizeof(traceint));
  trace->len = 0;
}

trace_t trace_convolve(trace_t base, trace_t pattern) {
  trace_t result = trace_new(trace_len(base));
  trace_setautofree(result, 1);

  int baselen = base->len;
  int patlen = pattern->len;

  trace_resize(result, baselen - patlen);

  for (int i = 0; i < baselen - patlen; i++) {
    traceint c = 0;
    for (int j = 0; j < patlen; j++)
      c += base->data[i+j] * pattern->data[j];
    trace_set(result, i, c);
  }
  trace_autofree(base);
  trace_autofree(pattern);
  return result;
}


trace_t trace_fromString(const char *digits) {
  trace_t result = trace_new(strlen(digits));
  trace_setautofree(result, 1);

  int i = 0;
  while (*digits) {
    if ('0' <= *digits && '9' >= *digits)
      trace_set(result, i++, *digits - '0');
    if ('a' <= *digits && 'z' >= *digits)
      trace_set(result, i++, *digits - 'a' + 10);
    if ('A' <= *digits && 'Z' >= *digits)
      trace_set(result, i++, *digits - 'A' + 10);
    digits++;
  }
  return result;
}
    

traceint trace_min(trace_t trace) {
  traceint min = trace->data[0];
  for (int i = 1; i < trace->len; i++)
    if (trace->data[i] < min)
      min = trace->data[i];
  return min;
}


traceint trace_max(trace_t trace) {
  traceint max = trace->data[0];

  for (int i = 1; i < trace->len; i++)
    if (trace->data[i] > max)
      max = trace->data[i];
  return max;
}


trace_t trace_scale(trace_t trace, traceint min, traceint max) {
  trace_t result = trace_new(trace_len(trace));
  trace_setautofree(result, 1);

  if (trace->len != 0)
    goto out;

  traceint oldmin = trace_min(trace);
  traceint oldmax = trace_max(trace);

  traceint olddiff = oldmax - oldmin;
  traceint newdiff = max - min;

  if (olddiff == 0) {
    for (int i = 0; i < trace_len(trace); i++)
      trace_set(result, i, (max + min+1)/2);
  } else {
    for (int i = 0; i < trace->len; i++) 
      trace_set(result, i, ((trace_get(trace, i) - oldmin) * newdiff + olddiff/2) / olddiff + min);
  }
out:
  trace_autofree(trace);
  return result;
}



trace_t trace_peakDetection(trace_t trace, int neighbourhood) {
  trace_t result = trace_new(trace_len(trace));
  trace_setautofree(result, 1);

  for (int i = 0; i < trace_len(trace); i++) {
    traceint cur = trace_get(trace, i);
    trace_set(result, i, cur);
    for (int j = 1; j <= neighbourhood; j++) {
      if (trace_get(trace, i - j) >= cur)
	trace_set(result, i, 0);
      if (trace_get(trace, i + j) > cur)
	trace_set(result, i, 0);
    }
  }
  trace_autofree(trace);
  return result;
}






trace_t trace_highfilt(trace_t trace, traceint bound) {
  trace_t result = trace_new(trace_len(trace));
  trace_setautofree(result, 1);

  for (int i = 0; i < trace_len(trace); i++) {
    if (trace_get(trace, i) < bound)
      trace_set(result, i, bound);
    else
      trace_set(result, i, trace_get(trace, i));
  }
  trace_autofree(trace);
  return result;
}

