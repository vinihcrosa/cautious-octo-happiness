from multiprocessing import Pool

def primo(n):
    for i in range(2, int(n ** 0.5) + 1):
        if n % i == 0:
            return False
    print(n)
    return True


def main():
    primos = []
    with open('primos.txt', 'r') as f:
        for linha in f:
            primos.append(int(linha))

    min = primos[-1]
    max = int(primos[-1] * 2)

    with Pool() as p:
        primos += p.map(primo, range(min, max))

    with open('primos.txt', 'a') as f:
        for i in primos:
            if i:
                f.write(str(i) + '\n')

if __name__ == '__main__':
    main()