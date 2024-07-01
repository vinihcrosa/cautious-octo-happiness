function sieveOfAtkin(limit: number): number[] {
  const primes: boolean[] = Array(limit + 1).fill(false);
  const sqrtLimit = Math.sqrt(limit);

  for (let x = 1; x <= sqrtLimit; x++) {
      for (let y = 1; y <= sqrtLimit; y++) {
          let n = 4 * x * x + y * y;
          if (n <= limit && (n % 12 === 1 || n % 12 === 5)) {
              primes[n] = !primes[n];
          }

          n = 3 * x * x + y * y;
          if (n <= limit && n % 12 === 7) {
              primes[n] = !primes[n];
          }

          n = 3 * x * x - y * y;
          if (x > y && n <= limit && n % 12 === 11) {
              primes[n] = !primes[n];
          }
      }
  }

  for (let n = 5; n <= sqrtLimit; n++) {
      if (primes[n]) {
          const k = n * n;
          for (let i = k; i <= limit; i += k) {
              primes[i] = false;
          }
      }
  }

  const result: number[] = [2, 3];
  for (let n = 5; n <= limit; n++) {
      if (primes[n]) {
          result.push(n);
      }
  }

  return result;
}

// Exemplo de uso:
const limit = 50000000;
console.log(`Primos até ${limit}:`, sieveOfAtkin(limit));
