#include <stdio.h>
#include <stdlib.h>

int collatz_step(int n);

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
    return 1; // Error: Incorrect number of arguments
  }

  int n = atoi(argv[1]);
  if (n <= 0)
  {
    return 1; // Error: Input must be a positive integer
  }

  while (n != 1)
  {
    n = collatz_step(n);
    printf("%d\n", n);
  }

  return 0; // Success
}

int collatz_step(int n)
{
  if (n % 2 == 0)
    return n / 2;
  else
    return 3 * n + 1;
}