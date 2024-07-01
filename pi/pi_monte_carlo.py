import sys
import time
from numba import jit
import numpy as np
import ctypes

inicio = time.time()

@jit(nopython=True, parallel=True)
def pi_monte_carlo(n):
    x = np.random.random(n)
    y = np.random.random(n)
    count = np.sum(x**2 + y**2 <= 1)
    return 4 * count / n


lib = ctypes.CDLL('./pi.so')

lib.pi_monte_carlo.argtypes = [ctypes.c_int]
lib.pi_monte_carlo.restype = ctypes.c_longdouble

def count_time_c(n):
    inicio = time.time()
    pi = lib.pi_monte_carlo(n)
    fim = time.time()
    return fim - inicio, pi

def count_time_py(n):
    inicio = time.time()
    pi = pi_monte_carlo(n)
    fim = time.time()
    return fim - inicio, pi

if len(sys.argv) < 2:
    print("Uso: python script.py <n>")
    sys.exit(1)

n = sys.argv[1]

py_times = []
py_pi = []
c_times = []
c_pi = []

n = int(n)

for i in range(n):
    t, pi = count_time_py(10 ** i)
    py_times.append(t)
    py_pi.append(pi)
    t, pi = count_time_c(10 ** i)
    c_times.append(t)
    c_pi.append(pi)

max_width = len(str(10 ** (len(py_times) - 1)))

print("n\tPython\tC")
for i in range(n):
    print("{:<{width}d} {:<15.10f} {:<15.10f}".format(10 ** i, py_times[i], c_times[i], width=max_width))

# for i in range(n):
#     print("{:<{width}d} {:<15.10f} {:<15.10f}".format(10 ** i, py_pi[i], c_pi[i], width=max_width))

fim = time.time()

print("Tempo total: ", fim - inicio)

f = open("resultados.txt", "w")
f.write("n\tPython\tC\n")
for i in range(n):
    f.write("{:<{width}d} {:<15.10f} {:<15.10f}\n".format(10 ** i, py_times[i], c_times[i], width=max_width))
f.write("Tempo total: " + str(fim - inicio))
