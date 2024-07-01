#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <math.h>

long double pi_monte_carlo(int n) {
    int64_t dentro_circulo = 0;
    for (int64_t i = 0; i < n; i++) {
        double x = (double)rand() / RAND_MAX;
        double y = (double)rand() / RAND_MAX;
        if (x * x + y * y <= 1) {
            dentro_circulo++;
        }
    }
    return 4.0 * dentro_circulo / n;
}

// int main() {
//     // Inicializa o gerador de números aleatórios com a semente baseada no tempo atual
//     srand(time(0));

//     int64_t numero;

//     clock_t inicio, fim;
    
//     printf("Por favor, insira o numero de iterações: ");
    
//     scanf("%lld", &numero);
    
//     inicio = clock();
//     double pi = pi_monte_carlo(pow(10, numero));
//     fim = clock();
//     printf("Valor de pi calculado: %f\n", pi);
//     printf("Tempo de execução: %f segundos\n", (double)(fim - inicio) / CLOCKS_PER_SEC);

//     return 0;
// }