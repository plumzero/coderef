
#include <stdio.h>

char smatrix[10][20];

int main()
{
  const char* str = "$GPRMC,204700,A,3403.868,N,11709.432,W,001.9,336.9,170698,013.6,E*6E";

  int i = 0;
  int j = 0;
  int k = 0;
  int l = 0;
  while (str[i]) {
    if (str[i] != ',') {
      smatrix[j][k] = str[i];
      k++;
    } else {
      j++;
      k = 0;
    }
    i++;
  }

  l = j + 1;

  for (int m = 0; m < l; m++) {
    printf("%d %s\n", m, smatrix[m]);
  }

  return 0;
}