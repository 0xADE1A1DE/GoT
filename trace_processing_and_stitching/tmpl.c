#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define LINELEN 10000

#define DATALEN 1000

int readline(int *out, int outl, char *in) {
  int l = 0;

  while (*in) {
    switch (*in) {
      case '1':
      case '@':
	*out++ = 1;
	break;
      case '0':
      case '.': 
	*out++ = 0;
	break;
      case ' ':
      case '\t':
      case '\n':
	break;
      default:
	fprintf(stderr, "Unrecognised character 0x%02x (%c)\n", *in, isprint(*in)?*in:'?');
	exit(1);
    }
    l++;
    if (l == outl)
      return l;
    in++;
  }
  return l;
}



char *template0 = "111111111";

char *template1 = "111111111111111";

int correlate(int *out, int *in, int len, int *tmpl, int tmpllen) {
  for (int i = 0; i < len; i++)
    out[i] = 0;

  for (int i = 0; i < len - tmpllen; i++)
    for (int j = 0; j < tmpllen; j++)
      out[i] += in[i+j] == tmpl[j];

  return len - tmpllen;
}



int main(int ac, char **av) {
  char buf[LINELEN];
  buf[LINELEN-1] = '\0';

  int tmpl0[DATALEN];
  int tmpl0len = readline(tmpl0, DATALEN, template0);
  int tmpl1[DATALEN];
  int tmpl1len = readline(tmpl1, DATALEN, template1);

  int data[DATALEN];
  int corr0[DATALEN];
  int corr1[DATALEN];


  while (fgets(buf, LINELEN-1, stdin) != NULL) {
    int dl = readline(data, DATALEN, buf);
    int cl0 = correlate(corr0, data, dl, tmpl0, tmpl0len);
    int cl1 = correlate(corr1, data, dl, tmpl1, tmpl1len);
    for (int i = 0; i < dl; i++)
      printf("%4d, %3d, %3d\n", data[i] * 100+500, corr0[i]*1000/tmpl0len, corr1[i]*1000/tmpl1len);
    printf("-------------------------\n");
  }
}

    

