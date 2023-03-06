#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cstring>

using namespace std;

int main(int argc, char** argv) {
  char *sm = argv[1];
  if (sm)
  {
    
    char b[100000];
    if (sm[strlen(sm) - 1] == 'S')
      sm[strlen(sm) - 1] = '\0';
    char *p, *q;
    for (p = sm, q = b; *p; p++)
    {
      if (*p != 'S')
        continue;
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
      
      printf("\n%s\n", b);
    }

    
  }
}