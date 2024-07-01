#include <stdio.h>
#include <stdlib.h>
#include <math.h>

int is_prime(int n)
{
  if (n < 2)
  {
    return 0;
  }
  for (int i = 2; i * i <= sqrt(n); i++)
  {
    if (n % i == 0)
    {
      return 0;
    }
  }
  return 1;
}

void write_file(FILE *f, int n)
{
  for (int i = 2; i < n; i++)
  {
    if (is_prime(i))
    {
      fprintf(f, "%d\n", i);
    }
  }
  fclose(f);
}

int get_max_prime()
{
  FILE *f = fopen("primes.txt", "r");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }
  int max_prime = 0;
  int prime;
  while (fscanf(f, "%d", &prime) != EOF)
  {
    max_prime = prime;
  }
  fclose(f);
  return max_prime;
}

int main(int argc, char *argv[])
{
  int n = 100000000000;
  int last_prime = 0;
  int primes = 0;
  if (argc > 1)
  {
    n = atoi(argv[1]);
  }

  int max_prime = get_max_prime();
  printf("Max prime: %d\n", max_prime);

  FILE *f = fopen("primes.txt", "a");
  if (f == NULL)
  {
    printf("Error opening file!\n");
    exit(1);
  }

  int max_num = max_prime + n;

  printf("Generating primes up to %d\n", max_num);

  for (int i = 2; i < max_num; i++)
  {
    if (is_prime(i))
    {
      last_prime = i;
      primes++;
      fprintf(f, "%d\n", i);
    }
  }

  printf("Primes: %d\n", primes);
  printf("Last prime: %d\n", last_prime);

  fclose(f);

  return 0;
}