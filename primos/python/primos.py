import sys;
import math;
from threading import Thread
import time
import timeit

inicio = timeit.default_timer()
anterior = timeit.default_timer()
print("anterio -> ", anterior)

primos = [];

arq = open("primos.txt", 'r');

for linha in arq:
  primos.append(int(linha));

initLen = len(primos);
print("Quantidade de primos no inicio -> ", initLen);
print("Inicio do calculo de primos -> ", primos[-1])
arq = open("primos.txt", 'a');

max = int(primos[-1] * 2);

def formataTempo(segundos):
  horas = segundos // 3600;
  segundos = segundos % 3600;
  minutos = segundos // 60;
  segundos = segundos % 60;
  return "{0:.3g}".format(horas) + "h " + "{0:.3g}".format(minutos) + "m " + "{0:.3g}".format(segundos) + "s";

def ePrimo(n):
    for i in primos:
      if i > math.sqrt(n):
        break;
      if n % i == 0:
        return False;
    return True;

def printPerCent(n, max):
  percent = (n/max)*100;
  agora = timeit.default_timer()
  print("anterio -> ", anterior)
  if(abs(agora - anterior) > 1):
    print("\r", '{0:.3g}'.format(percent), "%", "em -> ", formataTempo(agora - inicio), "seg           ",end="");
    sys.stdout.flush();
    anterior = agora;

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
print("ultimo primo achado -> ", primos[-1]);
print("quantidade de primos calculados -> ", endLen - initLen);

