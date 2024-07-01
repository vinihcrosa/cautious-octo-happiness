import sys;
import math;
from threading import Thread

class Primo(Thread):
  def __init__(self, num, primos):
    Thread.__init__(self)
    self.num = num
    self.primos = primos

  def run(self):
    if(self.ePrimo(self.num)):
      self.primo = True
      print(self.num)
    else:
      self.primo = False
  
  def ePrimo(self, n):
    for i in self.primos:
      if i > math.sqrt(n):
        break;
      if n % i == 0:
        return False;
    return True;

  def getEPrimos(self):
    return self.primo;


primos = [];

arq = open("primos.txt", 'r');

for linha in arq:
  primos.append(int(linha));

min = primos[-1];
max = int(primos[-1] * 1.1);

primo = Primo(50, primos);
primo.start();
print(primo.num);
print(primo.getEPrimos());

threads = []

for i in range(min, max):
  threads.append(Primo(i, primos));
  threads[-1].start();

for i in threads:
  if(threads[i].getEPrimos()):
    primos.append(threads[i].num);

print(primos);