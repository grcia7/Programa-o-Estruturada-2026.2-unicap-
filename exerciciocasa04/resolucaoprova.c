/*
 * UNICAP - Programação Estruturada (INF 1917) - 2º GQ - Junho/2026
 * Resolução comentada para estudo.
 *
 * Compilar: gcc -Wall -o prova resolucao_prova_INF1917.c
 *
 * A função main() no final serve apenas para TESTAR as questões.
 * Na prova, só as 5 funções (e o que elas usam) precisam ser entregues.
 */

#include <stdio.h>
#include <string.h>

#define TAM_CONTAS 50
#define TAM_OPERACOES 10000

struct Conta {
    int id;
    char cliente[40];
    double saldo;
    double limite;
};

struct Operacao {
    int id;
    int idConta;
    char tipo;      // {'D', 'S', 'T'}
    double valor;   // positivo = crédito, negativo = débito
};

/* ------------------------------------------------------------------ */
/* Função que a prova diz que JÁ EXISTE (não precisa reescrever na    */
/* prova; está aqui só para o programa compilar).                     */
/* ------------------------------------------------------------------ */
int buscaId(struct Conta v[], int tam, int x)
{
    int i;
    for (i = 0; i < tam; i++)
        if (v[i].id == x)
            return i;
    return -1;
}

/* ------------------------------------------------------------------ */
/* QUESTÃO 1 - cadastraConta                                          */
/* Ideia: validar TUDO antes de gravar no vetor. Só incrementa *tam   */
/* no final, depois que a conta foi realmente cadastrada.             */
/* ------------------------------------------------------------------ */
void cadastraConta(struct Conta v[], int *tam)
{
    int id;
    double limite;

    // 1) vetor cheio?
    if (*tam >= TAM_CONTAS) {
        printf("Erro: vetor de contas cheio.\n");
        return;
    }

    // 2) ID duplicado? (usa a buscaId fornecida)
    printf("ID da conta: ");
    scanf("%d", &id);
    if (buscaId(v, *tam, id) != -1) {
        printf("Erro: ja existe conta com esse ID.\n");
        return;
    }

    // 3) limite negativo?
    printf("Limite: ");
    scanf("%lf", &limite);
    if (limite < 0) {
        printf("Erro: limite nao pode ser negativo.\n");
        return;
    }

    // 4) tudo válido: grava na posição *tam e só então incrementa
    v[*tam].id = id;
    v[*tam].limite = limite;
    v[*tam].saldo = 0;                     // conta nova começa zerada
    printf("Nome do cliente: ");
    scanf(" %39[^\n]", v[*tam].cliente);   // lê com espaços, máx. 39 chars
    (*tam)++;

    printf("Conta cadastrada com sucesso!\n");
}

/* ------------------------------------------------------------------ */
/* QUESTÃO 2 - buscaBinariaNome                                       */
/* Pré-condição: o vetor está ORDENADO por cliente (ordem alfabética) */
/* Para strings usa-se strcmp (nunca == ou <):                        */
/*   strcmp(a,b) == 0 -> iguais | < 0 -> a vem antes | > 0 -> a depois */
/* ------------------------------------------------------------------ */
int buscaBinariaNome(struct Conta v[], int tam, char x[])
{
    int ini = 0, fim = tam - 1, meio, cmp;

    while (ini <= fim) {
        meio = (ini + fim) / 2;
        cmp = strcmp(x, v[meio].cliente);

        if (cmp == 0)
            return meio;          // achou
        else if (cmp < 0)
            fim = meio - 1;       // x vem antes -> metade esquerda
        else
            ini = meio + 1;       // x vem depois -> metade direita
    }
    return -1;                    // não achou
}

/* ------------------------------------------------------------------ */
/* QUESTÃO 3 - filtraDevedores                                        */
/* Ideia: percorrer vContas; se saldo < 0, copiar a conta inteira     */
/* para vDevedores. Atribuição de struct copia todos os campos.       */
/* ------------------------------------------------------------------ */
void filtraDevedores(struct Conta vContas[], int tamConta,
                     struct Conta vDevedores[], int *tamDevedores)
{
    int i;

    *tamDevedores = 0;            // IMPORTANTE: zerar antes de contar

    for (i = 0; i < tamConta; i++) {
        if (vContas[i].saldo < 0) {
            vDevedores[*tamDevedores] = vContas[i];
            (*tamDevedores)++;
        }
    }
}

/* ------------------------------------------------------------------ */
/* QUESTÃO 4 - realizarSaque                                          */
/* Regras: conta existe? saldo + limite >= valor?                     */
/* Se sim: debita o saldo, registra operação com valor NEGATIVO,      */
/* incrementa *tamOp e retorna 1. Caso contrário retorna 0.           */
/* ------------------------------------------------------------------ */
int realizarSaque(struct Conta vContas[], int tamConta,
                  struct Operacao vOp[], int *tamOp)
{
    int id, pos;
    double valor;

    printf("ID da conta: ");
    scanf("%d", &id);
    printf("Valor do saque: ");
    scanf("%lf", &valor);

    // valor inválido ou vetor de operações cheio
    if (valor <= 0 || *tamOp >= TAM_OPERACOES)
        return 0;

    // a conta existe?
    pos = buscaId(vContas, tamConta, id);
    if (pos == -1)
        return 0;

    // saldo + limite cobre o saque?
    if (vContas[pos].saldo + vContas[pos].limite < valor)
        return 0;

    // tudo ok: atualiza o saldo...
    vContas[pos].saldo -= valor;

    // ...e registra a operação (valor negativo)
    vOp[*tamOp].id = *tamOp + 1;          // id sequencial da operação
    vOp[*tamOp].idConta = id;
    vOp[*tamOp].tipo = 'S';
    vOp[*tamOp].valor = -valor;
    (*tamOp)++;

    return 1;
}

/* ------------------------------------------------------------------ */
/* QUESTÃO 5 - extrato                                                */
/*                                                                    */
/* Observação: o enunciado fala em "usar o método da questão 3", mas  */
/* a questão 3 é filtraDevedores. Provavelmente é erro de digitação   */
/* e se refere à busca de conta por ID, ou seja, a buscaId fornecida. */
/*                                                                    */
/* Ideia (duas passadas sobre vOp):                                   */
/*  1) contar quantas operações a conta tem (total)                   */
/*  2) somar as operações ANTERIORES às 5 últimas -> saldo anterior   */
/*  3) imprimir as (até) 5 últimas, atualizando o saldo a cada uma    */
/* ------------------------------------------------------------------ */

// Imprime valor sem sinal seguido de C (>= 0) ou D (< 0). Ex.: 424.99C
void imprimeValor(double v)
{
    if (v >= 0)
        printf("%7.2fC", v);
    else
        printf("%7.2fD", -v);
}

// Converte o tipo da operação no texto da coluna (já preenchido até 17 colunas;
// feito à mão porque printf conta bytes, e não letras acentuadas)
const char *descricaoOperacao(char tipo)
{
    if (tipo == 'D') return "Depósito         ";
    if (tipo == 'S') return "Saque            ";
    return                  "Transferência    ";
}

void extrato(struct Conta vContas[], int tamConta,
             struct Operacao vOp[], int *tamOp)
{
    int id, pos, i, k, total = 0, primeira;
    double saldo = 0;                 // saldo inicial da conta é zero

    printf("ID da conta: ");
    scanf("%d", &id);

    pos = buscaId(vContas, tamConta, id);
    if (pos == -1) {
        printf("Conta inexistente.\n");
        return;
    }

    // 1ª passada: quantas operações essa conta tem?
    for (i = 0; i < *tamOp; i++)
        if (vOp[i].idConta == id)
            total++;

    // índice (0, 1, 2...) da primeira operação que será exibida
    primeira = (total > 5) ? total - 5 : 0;

    // 2ª passada: soma as operações que NÃO serão exibidas (saldo anterior).
    // Ao sair do for, 'i' está logo depois da última operação "pulada".
    k = 0;
    for (i = 0; i < *tamOp && k < primeira; i++) {
        if (vOp[i].idConta == id) {
            saldo += vOp[i].valor;
            k++;
        }
    }

    // Cabeçalho
    printf("+----------------------------------------+\n");
    printf("| %-33s N.%03d|\n", "C O N T A", id);
    printf("+----------------------------------------+\n");
    // (com acento a borda direita pode sair 1 coluna deslocada; irrelevante na prova)
    printf("|CLIENTE: %-31s|\n", vContas[pos].cliente);
    printf("+----------------------------------------+\n");
    printf("| ID | O P E R A Ç Ã O | VALOR  | SALDO  |\n");
    printf("+----+-----------------+--------+--------+\n");

    // Linha do saldo anterior
    printf("|    |Saldo Anterior   |        |");
    imprimeValor(saldo);
    printf("|\n");

    // Últimas operações: continua de onde parou (i), atualizando o saldo
    for (; i < *tamOp; i++) {
        if (vOp[i].idConta == id) {
            saldo += vOp[i].valor;
            printf("|%04d|%s|", vOp[i].id, descricaoOperacao(vOp[i].tipo));
            imprimeValor(vOp[i].valor);   // C se positivo, D se negativo
            printf("|");
            imprimeValor(saldo);
            printf("|\n");
        }
    }

    // Rodapé
    printf("+----+-----------------+--------+--------+\n");
    printf("%32s|", "Saldo Atual ");
    imprimeValor(saldo);
    printf("|\n");
    printf("%32s+--------+\n", "");
}

/* ------------------------------------------------------------------ */
/* main APENAS PARA TESTE: reproduz o exemplo do extrato da prova.    */
/* ------------------------------------------------------------------ */
int main(void)
{
    // contas ordenadas por nome (necessário para a busca binária)
    struct Conta contas[TAM_CONTAS] = {
        {2, "Ana Lima",          -100.00, 500.0},
        {3, "Carlos Souza",        50.00, 200.0},
        {1, "Maria José Santos",  424.99, 1000.0}
    };
    int tamContas = 3;

    struct Operacao ops[TAM_OPERACOES] = {
        {  1, 1, 'D', 2437.55},   // saldo anterior da Maria
        { 10, 2, 'D',  500.00},
        { 25, 1, 'D', 1000.00},
        { 30, 2, 'S', -600.00},
        { 37, 1, 'D',  237.44},
        { 83, 1, 'T', -4000.00},
        { 90, 3, 'D',   50.00},
        { 95, 1, 'S',  -250.00},
        {112, 1, 'T', 1000.00}
    };
    int tamOps = 9;

    struct Conta devedores[TAM_CONTAS];
    int tamDev, pos, i;
    char nome[40];

    printf("=== Q5: extrato (digite 1) ===\n");
    extrato(contas, tamContas, ops, &tamOps);

    printf("\n=== Q4: saque (ex.: 1 e 500 -> sucesso) ===\n");
    printf("Retorno: %d\n", realizarSaque(contas, tamContas, ops, &tamOps));
    printf("=== Q4: saque (ex.: 1 e 5000 -> falha) ===\n");
    printf("Retorno: %d\n", realizarSaque(contas, tamContas, ops, &tamOps));

    printf("\n=== Q3: devedores ===\n");
    filtraDevedores(contas, tamContas, devedores, &tamDev);
    for (i = 0; i < tamDev; i++)
        printf("%d - %s - %.2f\n", devedores[i].id, devedores[i].cliente,
               devedores[i].saldo);

    printf("\n=== Q2: busca binaria ===\n");
    strcpy(nome, "Carlos Souza");
    pos = buscaBinariaNome(contas, tamContas, nome);
    printf("'%s' -> posicao %d\n", nome, pos);
    strcpy(nome, "Zeca");
    pos = buscaBinariaNome(contas, tamContas, nome);
    printf("'%s' -> posicao %d\n", nome, pos);

    printf("\n=== Q1: cadastro (ID novo, ex.: 4 / 300 / Bruno Dias) ===\n");
    cadastraConta(contas, &tamContas);
    printf("Total de contas: %d\n", tamContas);

    return 0;
}