#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>

using namespace std;

char *bitstosm(const char *bits)
{
  char *out = (char*)malloc(strlen(bits) * 2 + 1);
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

int main(int argc, char** argv) {
  char *sm = argv[1];
  char* sm_sequence = bitstosm(sm);
  printf("%s\n", sm_sequence);
}