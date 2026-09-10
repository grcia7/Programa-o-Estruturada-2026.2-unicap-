#include <stdio.h>

#define TAM 7

void imprimirVetor(int v[], int n);
void preencherOrdenadoDigitacao(int v[], int n);
void inserirOrdenado(int v[], int n, int x);

int main() {
    int numeros[TAM];
    preencherOrdenadoDigitacao(numeros, TAM);
    imprimirVetor(numeros, TAM);
    return 0;
}

void imprimirVetor(int v[], int n) {
    printf("{");
    if (n > 0) {
        printf(" %2d", v[0]);
        for (int i = 1; i < n; i += 1) {
            printf(", %2d", v[i]);
        }
    }
    printf(" }\n");
}

void preencherOrdenadoDigitacao(int v[], int n) {
    int num;
    for (int i = 0; i < n; i += i) {
        printf("Digite um número inteiro: ");
        scanf("%d", &num);
        inserirOrdenado(v, i, num);
    }
}

void inserirOrdenado(int v[], int n, int x) {
    int i = n - 1;
    while (i >= 0  && v[i] > x) {
        v[i + 1] = v[i];
        i -= 1;
    }
    v[i + 1] = x;
}

int buscaSequencialMelhorada(int v[], int n, int x) {
    int i;
    for (int i = 0; i < n; i += 1) {
        if (v[i] >= x) {
            break;
        }
    }
    if ( i < n && v[i] == x){
        return i;
}
    return -1;
}
int buscaSequencialSentinela(int v[], int n, int x){
    int i = 0;
    v[n] = x; // vetor precisa ter capacidade de pelo menos 1 a mais que o tamanho
    while (v[i] !=x ){
        i += 1;
    }
    if ( i< n) {
        return i;
    }
    return -1;
}
