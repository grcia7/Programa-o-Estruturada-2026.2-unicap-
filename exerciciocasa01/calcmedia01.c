#include <stdio.h>

#define TAM 30
#define PESO_NOTA1 2
#define PESO_NOTA2 3
#define NOTA_MINIMA 0
#define NOTA_MAXIMA 10

void preencherNotas(float nota1[], float nota2[], int n);
float lerNotaValida(void);
void calcularMedias(float nota1[], float nota2[], float media[], int n);
float calcularMediaTurma(float media[], int n);
void imprimirAlunosAcimaDaMedia(float nota1[], float nota2[], float media[], int n, float mediaTurma);

int main(void) {
    float nota1[TAM], nota2[TAM], media[TAM];
    float mediaTurma;

    printf("===== Preenchimento das Notas dos Alunos =====\n");
    preencherNotas(nota1, nota2, TAM);

    calcularMedias(nota1, nota2, media, TAM);

    mediaTurma = calcularMediaTurma(media, TAM);

    printf("\n===== Resultado =====\n");
    printf("A media da turma foi %.2f\n", mediaTurma);

    printf("\n===== Alunos acima da media da turma =====\n");
    imprimirAlunosAcimaDaMedia(nota1, nota2, media, TAM, mediaTurma);

    return 0;
}

void preencherNotas(float nota1[], float nota2[], int n) {
    for (int i = 0; i < n; i += 1) {
        printf("\n--- Aluno %d ---\n", i + 1);

        printf("Digite a nota 1 (0 a 10):\n");
        nota1[i] = lerNotaValida();

        printf("Digite a nota 2 (0 a 10):\n");
        nota2[i] = lerNotaValida();
    }
}

float lerNotaValida(void) {
    float nota;
    scanf("%f", &nota);

    while (nota < NOTA_MINIMA || nota > NOTA_MAXIMA) {
        printf("Nota invalida! Digite um valor entre %d e %d:\n", NOTA_MINIMA, NOTA_MAXIMA);
        scanf("%f", &nota);
    }

    return nota;
}

void calcularMedias(float nota1[], float nota2[], float media[], int n) {
    float somaPesos = PESO_NOTA1 + PESO_NOTA2;
    for (int i = 0; i < n; i += 1) {
        media[i] = (nota1[i] * PESO_NOTA1 + nota2[i] * PESO_NOTA2) / somaPesos;
    }
}

float calcularMediaTurma(float media[], int n) {
    float soma = 0;
    for (int i = 0; i < n; i += 1) {
        soma += media[i];
    }
    return soma / n;
}

void imprimirAlunosAcimaDaMedia(float nota1[], float nota2[], float media[], int n, float mediaTurma) {
    int existeAlunoAcima = 0;

    printf("%-6s | %-8s | %-8s | %-8s\n", "Aluno", "Nota1", "Nota2", "Media");
    printf("---------------------------------------\n");

    for (int i = 0; i < n; i += 1) {
        if (media[i] > mediaTurma) {
            printf("%-6d | %-8.2f | %-8.2f | %-8.2f\n",
                   i + 1, nota1[i], nota2[i], media[i]);
            existeAlunoAcima = 1;
        }
    }

    if (!existeAlunoAcima) {
        printf("Nenhum aluno ficou acima da media da turma.\n");
    }
}