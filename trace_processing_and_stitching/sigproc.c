#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "trace.h"

#define LINELEN 10000



char *getrandom();

char *bitstosm(const char *bits)
{
  char *out = malloc(strlen(bits) * 2 + 1);
  int j = 0;
  for (int i = 0; i < strlen(bits); i++)
  {
    out[j++] = 'S';
    if (bits[i] == '1')
      out[j++] = 'm';
  }
  out[j] = '\0';
  return out;
}

#define OFFSET 15

char *peakstosm(trace_t peaks, traceint thresh)
{
  char *out = malloc((trace_len(peaks) + 20) / 10);
  int p;
  for (p = 1; p < trace_len(peaks); p++)
    if (trace_get(peaks, p) >= thresh)
      break;
  if (p == trace_len(peaks))
    return NULL;

  out[0] = 'S';
  int i = 1;
  int pm = 0;
  while (1)
  {
    int q;
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    
    for (q = p + 15; q < p + 45; q++)
      if (trace_get(peaks, q) > thresh)
        break;
    if (q >= p + 45)
    {
      p += 31;
      if (p >= trace_len(peaks))
        break;
      out[i++] = pm ? '.' : 'm';
      pm = 1;
    }
    else
    {
      out[i++] = 'S';
      p = q;
      pm = 0;
    }
    
    
  }
  out[i] = '\0';
  return out;
}

int match1(char *hs, char *s)
{
  int r = 0;
  while (*s && *hs)
  {
    if (*s == *hs)
      r++;
    s++;
    hs++;
  }
  return r;
}

int match(char *hs, char *s)
{
  int m = 0;
  while (*hs)
  {
    int t = match1(hs, s);
    if (t > m)
      m = t;
    hs++;
  }
  return m;
}

int matchpos(char *hs, char *s)
{
  int p = 0;
  int m = 0;
  int mp = 0;
  while (*hs)
  {
    int t = match1(hs, s);
    if (t > m)
    {
      m = t;
      mp = p;
    }
    hs++;
    p++;
  }
  return mp;
}

int len(char *s)
{
  int r = 0;
  while (*s)
  {
    if (*s != '.')
      r++;
    s++;
  }
  return r;
}

int main(int ac, char **av)
{
  char *haystack = bitstosm(getrandom());

  traceint thresh = 12;
  if (ac == 2)
    thresh = atoi(av[1]);

  char buf[LINELEN];
  buf[LINELEN - 1] = '\0';

  trace_t pat9 = trace_fromString("111111111");
  trace_t pat15 = trace_fromString("111111111111111");
  trace_setautofree(pat9, 0);
  trace_setautofree(pat15, 0);

  while (fgets(buf, LINELEN - 1, stdin) != NULL)
  {
    trace_t tr = trace_fromString(buf);
    trace_setautofree(tr, 0);

    for (int i = 0; i < trace_len(tr); i++)
    {
      switch (trace_get(tr, i) == 1)
      {
      case 0:
        if (trace_get(tr, i - 1) == 1 && trace_get(tr, i + 1) == 1)
          trace_set(tr, i, 1);
        break;
      case 1:
        if (trace_get(tr, i - 2) == 0 && trace_get(tr, i - 1) == 0 && trace_get(tr, i + 1) == 0 && trace_get(tr, i + 2))
          trace_set(tr, i, 0);
        break;
      }
    }

    trace_t conv9 = trace_convolve(tr, pat9);
    trace_t conv15 = trace_convolve(tr, pat15);

    trace_t product = trace_new(trace_len(tr));

    for (int i = 0; i < trace_len(tr); i++)
      trace_set(product, i, trace_get(conv9, i) * trace_get(conv15, i));

    trace_t res = product;
    
    res = trace_peakDetection(res, 8);

#ifdef FORFIGURE
    for (int i = 0; i < trace_len(tr); i++)
      printf("%d, %d, %d\n", trace_get(tr, i), trace_get(product, i), trace_get(res, i));
#else

    char *sm = peakstosm(res, thresh);
    if (sm)
    {
      
      printf("%5d %2d %2d %2d %s\n", matchpos(haystack, sm), (int)strlen(sm), len(sm), match(haystack, sm), sm);

      char b[1000];
      if (sm[strlen(sm) - 1] == 'S')
        sm[strlen(sm) - 1] = '\0';
      char *p, *q;
      for (p = sm, q = b; *p; p++)
      {
        if (*p != 'S')
          break;
        if (p[1] == 'm')
        {
          *q = '1';
          p++;
        }
        else
        {
          *q = '0';
        }
        q++;
      }

      *q = '\0';
      if (*p != '\0')
      {
        
      }
      else
      {
        printf(">%4d %2d %2d %s\n", matchpos(getrandom(), b), (int)strlen(b), match(getrandom(), b), b);
      }

      free(sm);
    }

#endif

    trace_free(res);
    trace_free(conv9);
    trace_free(conv15);
    trace_free(tr);
  }
}











char *getrandom() {return "1001001100100011000111010100010101011001100010011011010110010111011101111110001111111010101000010010111101110111111010110000100000111010000111100110000000000111010001001000111010100001100010100000010010100111100111010101111000010011110001011110101100111111010001100011010000110010100110100110011010011001100010001001000011101101010101111101001111001001001000001011011101110101101110000100101011011101111001101010011010100010011001010010110000100111110110100"; }