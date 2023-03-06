#pragma once

typedef int traceint;


typedef struct trace *trace_t;


trace_t trace_new(int size);
void trace_free(trace_t trace);

void trace_setautofree(trace_t trace, int autofree);
int trace_getautofree(trace_t trace);


void trace_resize(trace_t trace, int newSize);





void trace_clear(trace_t trace);

trace_t  trace_fromString(const char *string);

trace_t trace_convolve(trace_t base, trace_t pattern);
trace_t trace_scale(trace_t trace, traceint min, traceint max);
trace_t trace_highfilt(trace_t trace, traceint bound);
trace_t trace_peakDetection(trace_t trace, int neighbourhood);

traceint trace_min(trace_t trace);
traceint trace_max(trace_t trace);






/*******************************************************/
struct trace {
  traceint *data;
  int len;
  uint32_t size;
  uint32_t flags;
};

#define TF_AUTOFREE 1
#define TF_AUTOFREED 2


inline void trace_set(trace_t trace, int index, traceint data) {
  if (index < 0)
    return;
  if (index >= trace->size)
    trace_resize(trace, index * 2);
  if (index >= trace->len)
    trace->len = index + 1;
  trace->data[index] = data;
}

inline traceint trace_get(trace_t trace, int index) {
  if (index < 0 || index >= trace->len)
    return 0;
  return trace->data[index];
}


inline int trace_len(trace_t trace) {
  return trace->len;
}



