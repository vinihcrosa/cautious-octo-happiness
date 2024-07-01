import sys;
import math;
import time
import timeit

inicio = timeit.default_timer()

primos = [];

arq = open("primos_grandes.txt", 'r');

for linha in arq:
  primos.append(int(linha));

arq = open("primos_grandes.txt", 'a');

def ePrimo(n):
    numMax = 10 ** (int(len(str(n)) / 2 + 1))
    for i in primos:
      if i > numMax:
        break;
      if n % i == 0:
        return False;
    return True;

primo1 = primos[-1]
primo2 = primos[-2]

print(len(str(primo2)))
print(len(str(primo1)))

for i in range(100):
  mult = primo1 * primo2;
  while not ePrimo(mult):
    mult += 1;
  agora = timeit.default_timer()
  print(len(str(mult)), " - ", agora - inicio, "s");
  arq.write(str(mult) + "\n");
  primo1 = primo2;
  primo2 = mult;
  inicio = agora;