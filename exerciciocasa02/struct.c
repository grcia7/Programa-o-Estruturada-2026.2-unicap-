#include <math.h>
#include <stdio.h>
#include <string.h>

// Compilar com: gcc ex11-14-todos.c -o todos -lm

#define TAM_TEXTO 100
#define TAM_NOME 50
#define TAM_ENDERECO 100

// ---------------------------------------------------------------
// Exercício 1: novos tipos de dados
// ---------------------------------------------------------------
struct Horario {
    int hora;
    int minutos;
    int segundos;
};

struct Data {
    int dia;
    int mes;
    int ano;
};

struct Compromisso {
    struct Data data;
    struct Horario horario;
    char texto[TAM_TEXTO];
};

// ---------------------------------------------------------------
// Exercício 2: pessoa com nome, idade e endereço
// ---------------------------------------------------------------
struct Pessoa {
    char nome[TAM_NOME];
    int idade;
    char endereco[TAM_ENDERECO];
};

// ---------------------------------------------------------------
// Exercício 3: vetor no R3
// ---------------------------------------------------------------
struct Vetor {
    double x;
    double y;
    double z;
};

// ---------------------------------------------------------------
// Exercício 4: coordenadas polares e cartesianas
// ---------------------------------------------------------------
struct PontoPolar {
    double r;  // raio
    double a;  // argumento, em radianos
};

struct PontoCartesiano {
    double x;
    double y;
};

// Funções auxiliares
void lerStr(char str[], int tamMax);
void limparBuffer();

// Exercício 1
void lerData(struct Data *d);
void lerHorario(struct Horario *h);
void lerCompromisso(struct Compromisso *c);
void imprimirCompromisso(struct Compromisso c);

// Exercício 2
void preencherDigitacaoPessoa(struct Pessoa *p);
void imprimirPessoa(struct Pessoa p);

// Exercício 3
void lerVetor(struct Vetor *v);
struct Vetor somarVetores(struct Vetor a, struct Vetor b);
void imprimirVetor(struct Vetor v);

// Exercício 4
void lerPontoPolar(struct PontoPolar *p);
struct PontoCartesiano polarParaCartesiano(struct PontoPolar p);
void imprimirPontoPolar(struct PontoPolar p);
void imprimirPontoCartesiano(struct PontoCartesiano c);

// Execução de cada exercício
void exercicio1();
void exercicio2();
void exercicio3();
void exercicio4();

int main() {
    int opcao;
    do {
        printf("\n===== MENU =====\n");
        printf("1 - Compromisso (Data, Horario)\n");
        printf("2 - Pessoa (nome, idade, endereço)\n");
        printf("3 - Soma de dois vetores\n");
        printf("4 - Polar para cartesiana\n");
        printf("0 - Sair\n");
        printf("Escolha uma opção:\n");
        scanf("%d", &opcao);
        limparBuffer();  // descarta o '\n' que o scanf deixou no teclado
        printf("\n");

        switch (opcao) {
            case 1: exercicio1(); break;
            case 2: exercicio2(); break;
            case 3: exercicio3(); break;
            case 4: exercicio4(); break;
            case 0: printf("Até logo!\n"); break;
            default: printf("Opção inválida!\n");
        }
    } while (opcao != 0);
    return 0;
}

// ---------------------------------------------------------------
// Execução dos exercícios
// ---------------------------------------------------------------
void exercicio1() {
    struct Compromisso compromisso;
    lerCompromisso(&compromisso);
    imprimirCompromisso(compromisso);
}

void exercicio2() {
    struct Pessoa pessoa;
    preencherDigitacaoPessoa(&pessoa);
    imprimirPessoa(pessoa);
}

void exercicio3() {
    struct Vetor v1, v2, soma;
    printf("Primeiro vetor:\n");
    lerVetor(&v1);
    printf("Segundo vetor:\n");
    lerVetor(&v2);
    soma = somarVetores(v1, v2);
    printf("A soma dos vetores é: ");
    imprimirVetor(soma);
}

void exercicio4() {
    struct PontoPolar polar;
    struct PontoCartesiano cartesiano;
    lerPontoPolar(&polar);
    cartesiano = polarParaCartesiano(polar);
    imprimirPontoPolar(polar);
    imprimirPontoCartesiano(cartesiano);
}

// ---------------------------------------------------------------
// Exercício 1
// ---------------------------------------------------------------
void lerData(struct Data *d) {
    printf("Digite a data (dd mm aaaa):\n");
    scanf("%d %d %d", &d->dia, &d->mes, &d->ano);
}

void lerHorario(struct Horario *h) {
    printf("Digite o horário (hh mm ss):\n");
    scanf("%d %d %d", &h->hora, &h->minutos, &h->segundos);
}

void lerCompromisso(struct Compromisso *c) {
    lerData(&c->data);
    lerHorario(&c->horario);
    limparBuffer();
    printf("Digite a descrição do compromisso:\n");
    lerStr(c->texto, TAM_TEXTO);
}

void imprimirCompromisso(struct Compromisso c) {
    printf("Compromisso em %02d/%02d/%04d às %02d:%02d:%02d - %s\n",
           c.data.dia, c.data.mes, c.data.ano,
           c.horario.hora, c.horario.minutos, c.horario.segundos,
           c.texto);
}

// ---------------------------------------------------------------
// Exercício 2
// ---------------------------------------------------------------
void preencherDigitacaoPessoa(struct Pessoa *p) {
    printf("Digite o seu nome:\n");
    lerStr(p->nome, TAM_NOME);
    printf("Digite a sua idade:\n");
    scanf("%d", &p->idade);
    limparBuffer();
    printf("Digite o seu endereço:\n");
    lerStr(p->endereco, TAM_ENDERECO);
}

void imprimirPessoa(struct Pessoa p) {
    printf("\nNome: %s\n", p.nome);
    printf("Idade: %d anos\n", p.idade);
    printf("Endereço: %s\n", p.endereco);
}

// ---------------------------------------------------------------
// Exercício 3
// ---------------------------------------------------------------
void lerVetor(struct Vetor *v) {
    printf("Digite x, y e z:\n");
    scanf("%lf %lf %lf", &v->x, &v->y, &v->z);
    limparBuffer();
}

struct Vetor somarVetores(struct Vetor a, struct Vetor b) {
    struct Vetor resultado;
    resultado.x = a.x + b.x;
    resultado.y = a.y + b.y;
    resultado.z = a.z + b.z;
    return resultado;
}

void imprimirVetor(struct Vetor v) {
    printf("(%.2f, %.2f, %.2f)\n", v.x, v.y, v.z);
}

// ---------------------------------------------------------------
// Exercício 4
// ---------------------------------------------------------------
void lerPontoPolar(struct PontoPolar *p) {
    printf("Digite o raio:\n");
    scanf("%lf", &p->r);
    printf("Digite o argumento (em radianos):\n");
    scanf("%lf", &p->a);
    limparBuffer();
}

struct PontoCartesiano polarParaCartesiano(struct PontoPolar p) {
    struct PontoCartesiano c;
    c.x = p.r * cos(p.a);
    c.y = p.r * sin(p.a);
    return c;
}

void imprimirPontoPolar(struct PontoPolar p) {
    printf("Coordenada polar: r = %.2f, a = %.2f rad\n", p.r, p.a);
}

void imprimirPontoCartesiano(struct PontoCartesiano c) {
    printf("Coordenada cartesiana: x = %.2f, y = %.2f\n", c.x, c.y);
}

// ---------------------------------------------------------------
// Funções auxiliares
// ---------------------------------------------------------------
void limparBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
    }
}

void lerStr(char str[], int tamMax) {
    fgets(str, tamMax, stdin);
    int tam = strlen(str);
    if (tam > 0 && str[tam - 1] == '\n') {
        str[tam - 1] = '\0';
    }
}