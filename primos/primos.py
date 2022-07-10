import sys;

primos = [];

arq = open("primos.txt", 'r');

for linha in arq:
  primos.append(int(linha));

print(len(primos));
arq = open("primos.txt", 'w');

max = int(primos[-1] * 1.1);

for i in primos:
  arq.write(str(i) + "\n");

def ePrimo(n):
    for i in primos:
      if i > n/2:
        break;
      if n % i == 0:
        return False;
    return True;

def printPerCent(n, max):
  print("\r", '{0:.3g}'.format((n/max)*100), "%", end="");
  sys.stdout.flush();

min = primos[-1]
for i in range(min, max):
  printPerCent(i - min, max - min)
  if(ePrimo(i)):
    primos.append(i);
    arq.write(str(i) + "\n");

print('/n')
print(len(primos));
