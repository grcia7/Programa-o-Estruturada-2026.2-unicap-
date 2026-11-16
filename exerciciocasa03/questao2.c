/* Lista 10 - IP2 - Questao 2: cadastro de produtos da Papelaria Escolar.
 * Compilar: gcc -std=c99 -Wall -Wextra -o questao2 questao2.c
 * Executar: ./questao2      ou      ./questao2 < digitacao_questao2.txt
 * Obs. item (e): a OBS2 diz "inferior", mas o caso descrito (perguntar e zerar o
 * estoque) so faz sentido se a quantidade desejada for SUPERIOR ao estoque. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <limits.h>
#ifdef _WIN32
#include <io.h>
#define isatty _isatty
#else
#include <unistd.h>
#endif

#define MAX_PRODUTOS 40
#define TAM_DESC     50
#define TAM_LINHA    128
#define SEM_LIMITE   INT_MIN

typedef struct {
    int    codigo;
    char   descricao[TAM_DESC];
    double valor_unitario;
    int    estoque;
} Produto;

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
            printf("Entrada invalida! Digite um numero (ex.: 12.50).\n");
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

int ler_confirmacao(const char *msg)           /* 1 = sim, 0 = nao */
{
    char l[TAM_LINHA], r;
    for (;;) {
        printf("%s", msg);
        ler_linha(l, sizeof l);
        r = toupper((unsigned char)l[0]);
        if (l[1] == '\0' && (r == 'S' || r == 'N')) return r == 'S';
        printf("Responda com S (sim) ou N (nao).\n");
    }
}

/* ------------------------ subprogramas do enunciado ----------------------- */

/* Posicao do produto no vetor, ou -1 se nao existir. */
int buscar_produto(const Produto p[], int qtd, int codigo)
{
    for (int i = 0; i < qtd; i++)
        if (p[i].codigo == codigo) return i;
    return -1;
}

/* Como buscar_produto, mas ja avisa o usuario quando nao encontra. */
int localizar_produto(const Produto p[], int qtd, int codigo)
{
    int pos = buscar_produto(p, qtd, codigo);
    if (pos < 0) printf("Produto %d nao cadastrado.\n", codigo);
    return pos;
}

/* (a) */
void cadastrar_produto(Produto p[], int *qtd)
{
    Produto n;
    if (*qtd >= MAX_PRODUTOS) {
        printf("Cadastro cheio! Limite de %d produtos.\n", MAX_PRODUTOS);
        return;
    }
    n.codigo = ler_inteiro("Codigo: ", 1);
    if (buscar_produto(p, *qtd, n.codigo) >= 0) {
        printf("Ja existe um produto com o codigo %d. Cadastro cancelado.\n", n.codigo);
        return;
    }
    ler_texto("Descricao: ", n.descricao, TAM_DESC);
    n.valor_unitario = ler_real("Valor unitario (R$): ", 0);
    n.estoque        = ler_inteiro("Quantidade em estoque: ", 0);
    p[(*qtd)++] = n;
    printf("Produto \"%s\" cadastrado com sucesso! (%d/%d)\n", n.descricao, *qtd, MAX_PRODUTOS);
}

/* (b) */
void alterar_valor(Produto p[], int qtd, int codigo)
{
    int pos = localizar_produto(p, qtd, codigo);
    if (pos < 0) return;
    printf("Produto: %s | valor atual: R$ %.2f\n", p[pos].descricao, p[pos].valor_unitario);
    p[pos].valor_unitario = ler_real("Novo valor unitario (R$): ", 0);
    printf("Valor alterado com sucesso! Novo valor: R$ %.2f\n", p[pos].valor_unitario);
}

/* (c) devolve -1 se o produto nao existir */
double consultar_valor(const Produto p[], int qtd, int codigo)
{
    int pos = buscar_produto(p, qtd, codigo);
    return pos < 0 ? -1.0 : p[pos].valor_unitario;
}

/* (d) devolve -1 se o produto nao existir */
int consultar_estoque(const Produto p[], int qtd, int codigo)
{
    int pos = buscar_produto(p, qtd, codigo);
    return pos < 0 ? -1 : p[pos].estoque;
}

/* (e) */
void vender_produto(Produto p[], int qtd, int codigo, int quantidade)
{
    int pos = localizar_produto(p, qtd, codigo);
    Produto *x;
    char pergunta[TAM_LINHA];

    if (pos < 0) return;
    x = &p[pos];
    if (x->estoque == 0) {
        printf("Produto \"%s\" esta com estoque zero. Venda nao realizada.\n", x->descricao);
        return;
    }
    if (quantidade > x->estoque) {
        printf("Estoque insuficiente! Desejado: %d | Disponivel: %d\n", quantidade, x->estoque);
        snprintf(pergunta, sizeof pergunta,
                 "Deseja comprar as %d unidade(s) disponivel(is)? (S/N): ", x->estoque);
        if (!ler_confirmacao(pergunta)) {
            printf("Compra cancelada. Estoque mantido em %d.\n", x->estoque);
            return;
        }
        quantidade = x->estoque;          /* leva tudo o que ha e zera o estoque */
    }
    x->estoque -= quantidade;
    printf("Venda realizada: %d x %s\nValor a ser pago: R$ %.2f\nEstoque restante: %d\n",
           quantidade, x->descricao, quantidade * x->valor_unitario, x->estoque);
}

/* (f) */
void atualizar_estoque(Produto p[], int qtd, int codigo)
{
    int pos = localizar_produto(p, qtd, codigo);
    if (pos < 0) return;
    printf("Produto: %s | estoque atual: %d\n", p[pos].descricao, p[pos].estoque);
    p[pos].estoque = ler_inteiro("Nova quantidade em estoque: ", 0);
    printf("Estoque atualizado com sucesso! Nova quantidade: %d\n", p[pos].estoque);
}

/* (g) */
void listar_produtos(const Produto p[], int qtd)
{
    if (qtd == 0) { printf("Nenhum produto cadastrado.\n"); return; }
    printf("%-10s %s\n", "CODIGO", "DESCRICAO");
    for (int i = 0; i < qtd; i++)
        printf("%-10d %s\n", p[i].codigo, p[i].descricao);
    printf("Total: %d produto(s).\n", qtd);
}

/* (h) */
void listar_estoque_zero(const Produto p[], int qtd)
{
    int n = 0;
    for (int i = 0; i < qtd; i++)
        if (p[i].estoque == 0) {
            if (n++ == 0) printf("%-10s %s\n", "CODIGO", "DESCRICAO");
            printf("%-10d %s\n", p[i].codigo, p[i].descricao);
        }
    if (n == 0) printf("Nenhum produto com estoque zero.\n");
    else        printf("Total: %d produto(s) sem estoque.\n", n);
}

/* --------------------------------- main ----------------------------------- */
int main(void)
{
    Produto produtos[MAX_PRODUTOS];
    int qtd = 0, opcao, codigo = 0, estoque;
    double valor;

    printf("=== PAPELARIA ESCOLAR - CADASTRO DE PRODUTOS ===\n");
    do {
        printf("\n=== MENU ===\n"
               "1-Cadastrar  2-Alterar valor  3-Consultar valor  4-Consultar estoque\n"
               "5-Vender  6-Atualizar estoque  7-Listar todos  8-Listar sem estoque  0-Sair\n");
        opcao = ler_inteiro("Opcao: ", SEM_LIMITE);
        printf("\n");
        if (opcao >= 2 && opcao <= 6)
            codigo = ler_inteiro("Codigo do produto: ", SEM_LIMITE);

        switch (opcao) {
        case 1: cadastrar_produto(produtos, &qtd); break;
        case 2: alterar_valor(produtos, qtd, codigo); break;
        case 3:
            valor = consultar_valor(produtos, qtd, codigo);
            if (valor < 0) printf("Produto %d nao cadastrado.\n", codigo);
            else           printf("Valor unitario do produto %d: R$ %.2f\n", codigo, valor);
            break;
        case 4:
            estoque = consultar_estoque(produtos, qtd, codigo);
            if (estoque < 0) printf("Produto %d nao cadastrado.\n", codigo);
            else             printf("Quantidade em estoque do produto %d: %d\n", codigo, estoque);
            break;
        case 5:
            vender_produto(produtos, qtd, codigo, ler_inteiro("Quantidade desejada: ", 1));
            break;
        case 6: atualizar_estoque(produtos, qtd, codigo); break;
        case 7: listar_produtos(produtos, qtd); break;
        case 8: listar_estoque_zero(produtos, qtd); break;
        case 0: printf("Encerrando o programa. Ate logo!\n"); break;
        default: printf("Opcao invalida! Escolha um numero de 0 a 8.\n");
        }
    } while (opcao != 0);
    return 0;
}