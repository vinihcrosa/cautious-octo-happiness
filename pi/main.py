import math

def libniz(n):
  soma: float = 0
  errors = []
  for i in range(1, n+1):
    termo = ((-1) ** i)/(2*i-1)
    soma += termo
    erro = math.pi -( 4 * soma)
    if (i % (n/100) == 0):
      print(f'{(i/n * 100):06.2f}% - Valor de Pi: {(4 * soma):.16f} - Erro: {erro:.20f}')
    errors.append(erro)
  return errors

def gauss_legendre(n):
  precision = 10 ** -n
  a = 1
  b = 1/math.sqrt(2)
  t = 1/4
  p = 1
  while abs(a - b) > precision:
    a_n = (a + b) / 2
    b = math.sqrt(a * b)
    t = t - p * ((a - a_n) ** 2)
    a = a_n
    p = 2 * p
    pi = ((a + b) ** 2) / (4 * t)
    print(f'Gaus-Legendre Valor de Pi: {pi:.16f} - Erro: {abs(a - b)}')
  return pi

if __name__ == '__main__':
  termos = int(input('Digite a ordem do numero de termos: '))
  gauss_legendre(termos)