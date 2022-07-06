#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  FILE *f = fopen(argv[2], "w");
  int n = atoi(argv[1]);
  double a = 0;
  double b = 1;
  double c = 0;
  double i = 0;
  for (i = 0; i < n; i++)
  {
    c = a + b;
    a = b;
    b = c;
    printf("%f: %f -> %f\n", i, c, c / a);
    fprintf(f, "%f: %f -> %f\n", i, c, c / a);
  }
  return 0;
};