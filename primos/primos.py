import sys;
import math;
from threading import Thread


primos = [];

arq = open("primos.txt", 'r');

for linha in arq:
  primos.append(int(linha));

initLen = len(primos);
print(initLen);
arq = open("primos.txt", 'w');

max = int(primos[-1] * 1.1);

for i in primos:
  arq.write(str(i) + "\n");

def ePrimo(n):
    for i in primos:
      if i > math.sqrt(n):
        break;
      if n % i == 0:
        return False;
    return True;

def printPerCent(n, max):
  percent = (n/max)*100;
  if(percent % 1 <= 0.01):
    print("\r", '{0:.3g}'.format((n/max)*100), "%", end="");
    sys.stdout.flush();

def main(min, max):
  printPerCent(i - min, max - min)
  if(ePrimo(i)):
    primos.append(i);
    arq.write(str(i) + "\n");

min = primos[-1]
for i in range(min, max):
  main(min, max)

print('/n')
endLen = len(primos);
print(endLen);
print(endLen - initLen);

