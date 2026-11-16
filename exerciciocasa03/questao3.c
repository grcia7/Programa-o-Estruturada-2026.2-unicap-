/* Lista 10 - IP2 - Questao 3: cadastros de contas do Banco Dinheiro Certo.
 * Dois cadastros (corrente e poupanca), cada um um vetor de ate 100 structs Conta
 * junto com o contador de contas (struct Cadastro), assim as funcoes recebem so
 * "o cadastro" e "o numero da conta", como pede o enunciado.
 * Compilar: gcc -std=c99 -Wall -Wextra -o questao3 questao3.c
 * Executar: ./questao3      ou      ./questao3 < digitacao_questao3.txt */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#else
#include <unistd.h>
#endif

#define MAX_CONTAS           100
#define TAM_LINHA            128
#define SEM_LIMITE           INT_MIN
#define CADASTRO_VAZIO       -1
#define CONTA_NAO_ENCONTRADA -2

typedef struct {
    int    numero;
    char   nome[50], cpf[15], telefone[20];
    double saldo;
} Conta;

typedef struct {
    Conta contas[MAX_CONTAS];
    int   qtd;
} Cadastro;

/* ---------------------- leitura e validacao de dados ---------------------- */

/* Le uma linha sem o '\n' (descartando o que exceder o buffer). Se a entrada
 * veio de arquivo, repete a linha lida na tela. */
void ler_linha(char *s, int tam)
{
    if (!fgets(s, tam, stdin)) { printf("\n[Fim da entrada]\n"); exit(0); }
    if (!strchr(s, '\n') && !feof(stdin))
        for (int c; (c = getchar()) != '\n' && c != EOF; ) ;
    s[strcspn(s, "\r\n")] = '\0';
    if (!isatty(0)) printf("%s\n", s);
}

/* Le um inteiro >= minimo (SEM_LIMITE aceita qualquer valor). */
int ler_inteiro(const char *msg, int minimo)
{
    char linha[TAM_LINHA], *fim;
    long v;
    for (;;) {
        printf("%s", msg);
        ler_linha(linha, sizeof linha);
        v = strtol(linha, &fim, 10);
        if (fim == linha || *fim != '\0' || v > INT_MAX)
            printf("Entrada invalida! Digite um numero inteiro.\n");
        else if (v < minimo)
            printf("Valor invalido! Informe um numero maior ou igual a %d.\n", minimo);
        else
            return (int)v;
    }
}

/* Le um real >= minimo (aceita virgula ou ponto). */
double ler_real(const char *msg, double minimo)
{
    char linha[TAM_LINHA], *fim, *virg;
    double v;
    for (;;) {
        printf("%s", msg);
        ler_linha(linha, sizeof linha);
        if ((virg = strchr(linha, ',')) != NULL) *virg = '.';
        v = strtod(linha, &fim);
        if (fim == linha || *fim != '\0')
            printf("Entrada invalida! Digite um numero (ex.: 150.00).\n");
        else if (v < minimo)
            printf("Valor invalido! Informe um valor maior ou igual a %.2f.\n", minimo);
        else
            return v;
    }
}

void ler_texto(const char *msg, char *dest, int tam)
{
    for (;;) {
        printf("%s", msg);
        ler_linha(dest, tam);
        if (dest[0] != '\0') return;
        printf("Este campo nao pode ficar vazio.\n");
    }
}

/* ------------------------ subprogramas do enunciado ----------------------- */

/* (a) Posicao da conta no cadastro; -1 se o cadastro esta vazio; -2 se nao achou. */
int buscar_conta(const Cadastro *c, int numero)
{
    if (c->qtd == 0) return CADASTRO_VAZIO;
    for (int i = 0; i < c->qtd; i++)
        if (c->contas[i].numero == numero) return i;
    return CONTA_NAO_ENCONTRADA;
}

/* Usa buscar_conta e, se a conta nao existir, ja avisa o usuario. */
int localizar_conta(const Cadastro *c, int numero)
{
    int pos = buscar_conta(c, numero);
    if (pos == CADASTRO_VAZIO)
        printf("O cadastro esta vazio: a conta %d nao esta cadastrada.\n", numero);
    else if (pos == CONTA_NAO_ENCONTRADA)
        printf("A conta %d nao esta cadastrada.\n", numero);
    return pos;
}

/* (b) */
void cadastrar_conta(Cadastro *c, int numero)
{
    Conta *n;
    if (buscar_conta(c, numero) >= 0) {
        printf("Conta %d ja cadastrada! Nao e possivel cadastrar numero de conta repetido.\n",
               numero);
        return;
    }
    if (c->qtd >= MAX_CONTAS) {
        printf("Cadastro cheio! Limite de %d contas.\n", MAX_CONTAS);
        return;
    }
    n = &c->contas[c->qtd];
    n->numero = numero;
    ler_texto("Nome do cliente: ", n->nome, sizeof n->nome);
    ler_texto("CPF: ", n->cpf, sizeof n->cpf);
    ler_texto("Telefone para contato: ", n->telefone, sizeof n->telefone);
    n->saldo = ler_real("Saldo inicial (R$): ", 0);
    c->qtd++;                                   /* insere no final do cadastro */
    printf("Conta %d cadastrada com sucesso! (%d/%d)\n", numero, c->qtd, MAX_CONTAS);
}

/* (c) */
void consultar_saldo(const Cadastro *c, int numero)
{
    int pos = localizar_conta(c, numero);
    if (pos < 0) return;
    printf("Conta %d | Titular: %s\nSaldo: R$ %.2f\n", numero, c->contas[pos].nome,
           c->contas[pos].saldo);
}

/* (d) */
void depositar(Cadastro *c, int numero)
{
    int pos = localizar_conta(c, numero);
    if (pos < 0) return;
    c->contas[pos].saldo += ler_real("Valor do deposito (R$): ", 0.01);
    printf("Deposito realizado! Novo saldo: R$ %.2f\n", c->contas[pos].saldo);
}

/* (e) */
void sacar(Cadastro *c, int numero)
{
    int pos = localizar_conta(c, numero);
    double valor;
    if (pos < 0) return;
    valor = ler_real("Valor do saque (R$): ", 0.01);
    if (valor > c->contas[pos].saldo) {
        printf("Saldo insuficiente! Saldo atual: R$ %.2f\n", c->contas[pos].saldo);
        return;
    }
    c->contas[pos].saldo -= valor;
    printf("Saque realizado! Novo saldo: R$ %.2f\n", c->contas[pos].saldo);
}

/* (f) */
void exibir_contas(const Cadastro *c)
{
    if (c->qtd == 0) { printf("Nenhuma conta cadastrada.\n"); return; }
    printf("%-8s %-28s %s\n", "CONTA", "TITULAR", "TELEFONE");
    for (int i = 0; i < c->qtd; i++)
        printf("%-8d %-28s %s\n", c->contas[i].numero, c->contas[i].nome, c->contas[i].telefone);
    printf("Total: %d conta(s).\n", c->qtd);
}

/* ------------------------------ menus e main ------------------------------ */

/* Operacoes sobre o cadastro escolhido (corrente ou poupanca). */
void menu_conta(Cadastro *c, const char *titulo)
{
    int opcao, numero = 0;
    do {
        printf("\n--- %s ---\n"
               "1-Cadastrar  2-Saldo  3-Depositar  4-Sacar  5-Listar  0-Voltar\n", titulo);
        opcao = ler_inteiro("Opcao: ", SEM_LIMITE);
        printf("\n");
        if (opcao >= 1 && opcao <= 4)
            numero = ler_inteiro(opcao == 1 ? "Numero da nova conta: " : "Numero da conta: ",
                                 opcao == 1 ? 1 : SEM_LIMITE);
        switch (opcao) {
        case 1: cadastrar_conta(c, numero); break;
        case 2: consultar_saldo(c, numero); break;
        case 3: depositar(c, numero);       break;
        case 4: sacar(c, numero);           break;
        case 5: exibir_contas(c);           break;
        case 0: printf("Voltando ao menu principal.\n"); break;
        default: printf("Opcao invalida! Escolha um numero de 0 a 5.\n");
        }
    } while (opcao != 0);
}

int main(void)
{
    Cadastro corrente = {0}, poupanca = {0};
    int opcao;

    printf("=== BANCO DINHEIRO CERTO - CONTAS ===\n");
    do {
        printf("\n=== MENU PRINCIPAL ===\n1-Conta Corrente  2-Conta Poupanca  0-Sair\n");
        opcao = ler_inteiro("Opcao: ", SEM_LIMITE);
        if      (opcao == 1) menu_conta(&corrente, "CONTA CORRENTE");
        else if (opcao == 2) menu_conta(&poupanca, "CONTA POUPANCA");
        else if (opcao == 0) printf("\nEncerrando o programa. Ate logo!\n");
        else                 printf("\nOpcao invalida! Escolha 0, 1 ou 2.\n");
    } while (opcao != 0);
    return 0;
}