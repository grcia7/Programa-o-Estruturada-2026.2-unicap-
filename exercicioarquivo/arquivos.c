/*
 * Streaming de Videos - manipulacao de arquivos binarios em C
 *
 *   videos.bin   : registros Video
 *   usuarios.bin : registros Usuario (vetor favoritos[] com IDs de videos.bin)
 *
 * Campo-chave: "int id", sempre o PRIMEIRO campo dos dois structs. Por isso as
 * funcoes genericas (buscar, remover, proximo_id) leem o ID de qualquer
 * registro pela uniao Reg, sem precisar saber o tipo.
 *
 * Compilar: gcc -Wall -Wextra -std=c99 -o streaming main.c
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define ARQ_VIDEOS    "videos.bin"
#define ARQ_USUARIOS  "usuarios.bin"
#define ARQ_TEMP      "temp.bin"
#define MAX_FAVORITOS 20
#define QTD(v)        (sizeof (v) / sizeof (v)[0])

typedef struct {
    int  id;                        /* CAMPO-CHAVE; deve ser o primeiro campo */
    char titulo[100];
    char genero[40];
    int  ano;
    int  duracao_min;
} Video;

typedef struct {
    int  id;                        /* CAMPO-CHAVE; deve ser o primeiro campo */
    char nome[100];
    char email[100];
    int  qtd_favoritos;             /* posicoes em uso de favoritos[] */
    int  favoritos[MAX_FAVORITOS];  /* IDs de registros de videos.bin */
} Usuario;

/* Buffer para ler qualquer registro; "id" coincide com o 1o campo dos structs. */
typedef union { int id; Video v; Usuario u; } Reg;

enum { VIDEO, USUARIO };            /* indices das tabelas abaixo */
static const char  *arquivos[] = { ARQ_VIDEOS, ARQ_USUARIOS };
static const size_t tamanhos[] = { sizeof(Video), sizeof(Usuario) };
static const char  *nomes[]    = { "Video", "Usuario" };

/* ------------------------------ Entrada de dados ------------------------- */

/* Le uma linha sem o '\n'; descarta o excesso se for maior que o buffer. */
static void ler_linha(const char *prompt, char *dest, size_t tam) {
    size_t n;

    printf("%s", prompt);
    fflush(stdout);
    if (fgets(dest, (int)tam, stdin) == NULL) {
        printf("\nEntrada encerrada.\n");
        exit(0);
    }
    n = strcspn(dest, "\r\n");
    if (dest[n] == '\0') {
        int c;
        while ((c = getchar()) != '\n' && c != EOF)
            ;
    }
    dest[n] = '\0';
}

/* Texto: se o campo ja tem valor (atualizacao) ENTER o mantem; se esta vazio
 * (cadastro) o preenchimento e obrigatorio. */
static void pedir_texto(const char *prompt, char *campo, size_t tam) {
    char buf[128];

    for (;;) {
        ler_linha(prompt, buf, tam);
        if (buf[0] != '\0') break;
        if (campo[0] != '\0') return;
        printf("O campo nao pode ficar vazio.\n");
    }
    snprintf(campo, tam, "%s", buf);
}

/* Inteiro em [min, max]. Se 'atual' nao for NULL, ENTER mantem o valor atual. */
static int pedir_int(const char *prompt, int min, int max, const int *atual) {
    char buf[64], *fim;
    long v;

    for (;;) {
        ler_linha(prompt, buf, sizeof buf);
        if (buf[0] == '\0' && atual != NULL) return *atual;
        v = strtol(buf, &fim, 10);
        if (fim != buf) {
            while (*fim == ' ') fim++;
            if (*fim == '\0' && v >= min && v <= max) return (int)v;
        }
        printf("Valor invalido. Digite um numero entre %d e %d.\n", min, max);
    }
}

static int confirmar(const char *pergunta) {
    char b[8];

    ler_linha(pergunta, b, sizeof b);
    return b[0] == 's' || b[0] == 'S';
}

static void pausar(void) {
    char b[8];

    ler_linha("\nPressione ENTER para continuar...", b, sizeof b);
}

/* ------------------- Operacoes genericas em arquivos binarios ------------ */

/* Le UM registro. Se fread devolver menos que o pedido, feof() diz se foi o
 * fim do arquivo (normal) ou erro de leitura. Retorna 1 se leu, 0 se nao. */
static int ler_registro(FILE *f, void *dest, size_t tam) {
    if (fread(dest, tam, 1, f) == 1) return 1;
    if (feof(f) == 0) printf("Erro de leitura no arquivo.\n");
    return 0;
}

/* LEITURA especifica: devolve a posicao (indice) do registro com o ID, ou -1.
 * Se 'saida' nao for NULL, copia o registro para ela. */
static long buscar_registro(const char *arq, size_t tam, int id, void *saida) {
    FILE *f = fopen(arq, "rb");
    Reg r;
    long pos = -1, i = 0;

    if (f == NULL) return -1;
    while (pos < 0 && ler_registro(f, &r, tam)) {
        if (r.id == id) {
            pos = i;
            if (saida != NULL) memcpy(saida, &r, tam);
        }
        i++;
    }
    fclose(f);
    return pos;
}

/* Proximo ID livre = maior ID do arquivo + 1 (1 se o arquivo estiver vazio). */
static int proximo_id(const char *arq, size_t tam) {
    FILE *f = fopen(arq, "rb");
    Reg r;
    int maior = 0;

    if (f == NULL) return 1;
    while (ler_registro(f, &r, tam))
        if (r.id > maior) maior = r.id;
    fclose(f);
    return maior + 1;
}

/* ESCRITA de 'qtd' registros: modo "ab" = anexar ao fim, "wb" = recriar arquivo. */
static int gravar(const char *arq, const char *modo, const void *dados, size_t tam, size_t qtd) {
    FILE *f = fopen(arq, modo);
    size_t gravados;

    if (f == NULL) return 0;
    gravados = fwrite(dados, tam, qtd, f);
    return fclose(f) == 0 && gravados == qtd;
}

/* ATUALIZACAO: posiciona no inicio do registro (pos * tamanho) e sobrescreve. */
static int sobrescrever(const char *arq, long pos, const void *reg, size_t tam) {
    FILE *f = fopen(arq, "r+b");
    int ok;

    if (f == NULL) return 0;
    ok = fseek(f, pos * (long)tam, SEEK_SET) == 0 && fwrite(reg, tam, 1, f) == 1;
    return fclose(f) == 0 && ok;
}

/* REMOCAO: copia tudo menos o registro para temp.bin, fecha os dois arquivos,
 * apaga o antigo e renomeia o novo. Retorna 1 = removido, 0 = ID nao existe,
 * -1 = erro (o arquivo original so e apagado se tudo deu certo). */
static int remover_registro(const char *arq, size_t tam, int id) {
    FILE *orig = fopen(arq, "rb"), *novo;
    Reg r;
    int removido = 0, erro = 0;

    if (orig == NULL) return 0;
    if ((novo = fopen(ARQ_TEMP, "wb")) == NULL) {
        fclose(orig);
        return -1;
    }
    while (ler_registro(orig, &r, tam)) {
        if (r.id == id)
            removido = 1;                               /* pula este registro */
        else if (fwrite(&r, tam, 1, novo) != 1) {
            erro = 1;
            break;
        }
    }
    if (feof(orig) == 0) erro = 1;                      /* nao chegou ao fim */
    fclose(orig);
    if (fclose(novo) != 0) erro = 1;                    /* novo pode estar incompleto */
    if (erro || !removido) {
        remove(ARQ_TEMP);
        return erro ? -1 : 0;
    }
    if (remove(arq) != 0) {
        printf("Falha ao tentar remover o arquivo %s\n", arq);
        return -1;
    }
    if (rename(ARQ_TEMP, arq) != 0) {
        printf("Falha ao tentar renomear o arquivo. Os dados estao em %s\n", ARQ_TEMP);
        return -1;
    }
    return 1;
}

static int arquivo_tem_registros(const char *arq) {
    FILE *f = fopen(arq, "rb");
    long tamanho = 0;

    if (f != NULL) {
        if (fseek(f, 0, SEEK_END) == 0) tamanho = ftell(f);   /* fim do arquivo */
        fclose(f);
    }
    return tamanho > 0;
}

/* ------------------------ Regras do relacionamento ----------------------- */

/* Posicao do video no vetor de favoritos do usuario, ou -1. */
static int indice_favorito(const Usuario *u, int id_video) {
    int i;

    for (i = 0; i < u->qtd_favoritos; i++)
        if (u->favoritos[i] == id_video) return i;
    return -1;
}

/* Adiciona aos favoritos validando limite, existencia e duplicidade. */
static int adicionar_favorito(Usuario *u, int id_video) {
    if (u->qtd_favoritos >= MAX_FAVORITOS)
        printf("Limite de %d favoritos atingido.\n", MAX_FAVORITOS);
    else if (buscar_registro(ARQ_VIDEOS, sizeof(Video), id_video, NULL) < 0)
        printf("Video ID %d nao existe.\n", id_video);
    else if (indice_favorito(u, id_video) >= 0)
        printf("Esse video ja esta nos favoritos.\n");
    else {
        u->favoritos[u->qtd_favoritos++] = id_video;
        return 1;
    }
    return 0;
}

static int remover_favorito(Usuario *u, int id_video) {
    int i = indice_favorito(u, id_video);

    if (i < 0) return 0;
    for (; i < u->qtd_favoritos - 1; i++)
        u->favoritos[i] = u->favoritos[i + 1];
    u->favoritos[--u->qtd_favoritos] = 0;
    return 1;
}

/* Integridade referencial: ao remover um video, tira o ID dos favoritos de
 * todos os usuarios (atualizacao no proprio arquivo). Retorna qtd. alterada. */
static int limpar_favoritos_de_video(int id_video) {
    FILE *f = fopen(ARQ_USUARIOS, "r+b");
    Usuario u;
    int afetados = 0;

    if (f == NULL) return 0;
    while (ler_registro(f, &u, sizeof u)) {
        if (!remover_favorito(&u, id_video)) continue;
        /* SEEK_CUR negativo: volta ao inicio do registro recem-lido (cast p/
         * long ANTES do sinal, pois sizeof e size_t, sem sinal). */
        if (fseek(f, -(long)sizeof u, SEEK_CUR) != 0) {
            printf("Falha ao tentar posicionar ponteiro\n");
            break;
        }
        /* fflush entre a escrita e a proxima leitura (exigido em "r+b"). */
        if (fwrite(&u, sizeof u, 1, f) != 1 || fflush(f) != 0) {
            printf("Erro na gravacao do registro\n");
            break;
        }
        afetados++;
    }
    if (fclose(f) != 0) printf("Erro no fechamento do arquivo\n");
    return afetados;
}

/* ------------------------------- Impressao ------------------------------- */

/* Uma linha por registro (usuario: so os IDs dos favoritos, como no arquivo). */
static void imprimir(int tipo, const void *reg) {
    const Video *v = reg;
    const Usuario *u = reg;
    int i;

    if (tipo == VIDEO) {
        printf("  [%d] %s | genero: %s | ano: %d | duracao: %d min\n",
               v->id, v->titulo, v->genero, v->ano, v->duracao_min);
        return;
    }
    printf("  [%d] %s <%s> | favoritos (IDs):", u->id, u->nome, u->email);
    for (i = 0; i < u->qtd_favoritos; i++) printf(" %d", u->favoritos[i]);
    puts(u->qtd_favoritos ? "" : " nenhum");
}

/* Usuario com os TITULOS dos favoritos: cruza com videos.bin. */
static void imprimir_usuario_com_favoritos(const Usuario *u) {
    Video v;
    int i;

    printf("Usuario [%d] %s <%s>\n", u->id, u->nome, u->email);
    if (u->qtd_favoritos == 0) printf("    (nenhum video favoritado)\n");
    for (i = 0; i < u->qtd_favoritos; i++) {
        if (buscar_registro(ARQ_VIDEOS, sizeof v, u->favoritos[i], &v) >= 0)
            printf("    - %s (%s, %d)\n", v.titulo, v.genero, v.ano);
        else
            printf("    - [video ID %d nao encontrado]\n", u->favoritos[i]);
    }
}

/* ----------- Operacoes de menu iguais para videos e usuarios ------------- */

static int listar(int tipo) {
    FILE *f = fopen(arquivos[tipo], "rb");
    Reg r;
    int total = 0;

    if (f == NULL) return 0;
    while (ler_registro(f, &r, tamanhos[tipo])) {
        imprimir(tipo, &r);
        total++;
    }
    fclose(f);
    return total;
}

static void listar_todos(int tipo) {
    printf("\n--- Todos os registros de %s ---\n", arquivos[tipo]);
    if (listar(tipo) == 0) printf("Nenhum registro encontrado.\n");
}

static void buscar(int tipo) {
    Reg r;
    int id = pedir_int("ID: ", 1, INT_MAX, NULL);

    if (buscar_registro(arquivos[tipo], tamanhos[tipo], id, &r) < 0)
        printf("%s %d nao encontrado.\n", nomes[tipo], id);
    else if (tipo == USUARIO)
        imprimir_usuario_com_favoritos(&r.u);
    else
        imprimir(tipo, &r);
}

static void remover(int tipo) {
    Reg r;
    int id = pedir_int("ID a remover: ", 1, INT_MAX, NULL), afetados;

    if (buscar_registro(arquivos[tipo], tamanhos[tipo], id, &r) < 0) {
        printf("%s %d nao encontrado.\n", nomes[tipo], id);
        return;
    }
    imprimir(tipo, &r);
    if (!confirmar("Confirma a remocao? (s/N): ")) {
        printf("Remocao cancelada.\n");
        return;
    }
    if (remover_registro(arquivos[tipo], tamanhos[tipo], id) != 1) {
        printf("Erro ao remover o registro.\n");
        return;
    }
    printf("%s removido com sucesso!\n", nomes[tipo]);
    if (tipo == VIDEO && (afetados = limpar_favoritos_de_video(id)) > 0)
        printf("O video tambem foi retirado dos favoritos de %d usuario(s).\n", afetados);
}

/* ------------------------- Cadastro e atualizacao ------------------------ */

static void cadastrar_video(void) {
    Video v = {0};

    v.id = proximo_id(ARQ_VIDEOS, sizeof v);
    printf("\n--- Cadastro de video (ID %d) ---\n", v.id);
    pedir_texto("Titulo: ", v.titulo, sizeof v.titulo);
    pedir_texto("Genero: ", v.genero, sizeof v.genero);
    v.ano = pedir_int("Ano de lancamento: ", 1888, 2100, NULL);
    v.duracao_min = pedir_int("Duracao (minutos): ", 1, 100000, NULL);
    puts(gravar(ARQ_VIDEOS, "ab", &v, sizeof v, 1) ? "Video cadastrado com sucesso!"
                                                    : "Erro ao gravar o video.");
}

static void cadastrar_usuario(void) {
    Usuario u = {0};
    int id_video;

    u.id = proximo_id(ARQ_USUARIOS, sizeof u);
    printf("\n--- Cadastro de usuario (ID %d) ---\n", u.id);
    pedir_texto("Nome: ", u.nome, sizeof u.nome);
    pedir_texto("E-mail: ", u.email, sizeof u.email);
    printf("\nVideos disponiveis:\n");
    if (listar(VIDEO) == 0) printf("  (nenhum video cadastrado)\n");
    while (u.qtd_favoritos < MAX_FAVORITOS) {
        id_video = pedir_int("ID do video favorito (0 = terminar): ", 0, INT_MAX, NULL);
        if (id_video == 0) break;
        if (adicionar_favorito(&u, id_video)) printf("  Favorito adicionado.\n");
    }
    puts(gravar(ARQ_USUARIOS, "ab", &u, sizeof u, 1) ? "Usuario cadastrado com sucesso!"
                                                      : "Erro ao gravar o usuario.");
}

static void atualizar_video(void) {
    Video v;
    long pos;
    int id = pedir_int("ID do video a atualizar: ", 1, INT_MAX, NULL);

    if ((pos = buscar_registro(ARQ_VIDEOS, sizeof v, id, &v)) < 0) {
        printf("Video %d nao encontrado.\n", id);
        return;
    }
    printf("\nDados atuais:\n");
    imprimir(VIDEO, &v);
    printf("\n(ENTER mantem o valor atual)\n");
    pedir_texto("Novo titulo: ", v.titulo, sizeof v.titulo);
    pedir_texto("Novo genero: ", v.genero, sizeof v.genero);
    v.ano = pedir_int("Novo ano: ", 1888, 2100, &v.ano);
    v.duracao_min = pedir_int("Nova duracao (min): ", 1, 100000, &v.duracao_min);
    puts(sobrescrever(ARQ_VIDEOS, pos, &v, sizeof v) ? "Video atualizado com sucesso!"
                                                     : "Erro ao atualizar o arquivo.");
}

static void atualizar_usuario(void) {
    Usuario u;
    long pos;
    int opcao, ok, id = pedir_int("ID do usuario a atualizar: ", 1, INT_MAX, NULL);

    if ((pos = buscar_registro(ARQ_USUARIOS, sizeof u, id, &u)) < 0) {
        printf("Usuario %d nao encontrado.\n", id);
        return;
    }
    do {
        printf("\nDados atuais:\n");
        imprimir_usuario_com_favoritos(&u);
        printf("\n  1 - Alterar nome / e-mail\n  2 - Adicionar video aos favoritos\n"
               "  3 - Remover video dos favoritos\n  0 - Voltar\n");
        opcao = pedir_int("Opcao: ", 0, 3, NULL);
        ok = 0;
        if (opcao == 1) {
            printf("(ENTER mantem o valor atual)\n");
            pedir_texto("Novo nome: ", u.nome, sizeof u.nome);
            pedir_texto("Novo e-mail: ", u.email, sizeof u.email);
            ok = 1;
        } else if (opcao == 2) {
            printf("\nVideos disponiveis:\n");
            listar(VIDEO);
            ok = adicionar_favorito(&u, pedir_int("ID do video a favoritar: ", 1, INT_MAX, NULL));
        } else if (opcao == 3) {
            ok = remover_favorito(&u, pedir_int("ID do video a remover: ", 1, INT_MAX, NULL));
            if (!ok) printf("Esse video nao esta nos favoritos do usuario.\n");
        }
        if (ok)
            puts(sobrescrever(ARQ_USUARIOS, pos, &u, sizeof u) ? "Usuario atualizado com sucesso!"
                                                               : "Erro ao atualizar o arquivo.");
    } while (opcao != 0);
}

/* ------------------------ Relatorios (cruzam os arquivos) ---------------- */

static void relatorio_usuarios_favoritos(void) {
    FILE *f = fopen(ARQ_USUARIOS, "rb");
    Usuario u;
    int total = 0;

    printf("\n=========== RELATORIO: USUARIOS E SEUS VIDEOS FAVORITOS ===========\n\n");
    while (f != NULL && ler_registro(f, &u, sizeof u)) {
        imprimir_usuario_com_favoritos(&u);
        printf("\n");
        total++;
    }
    if (f != NULL) fclose(f);
    printf("Total de usuarios: %d\n", total);
}

static void relatorio_videos_fas(void) {
    FILE *f = fopen(ARQ_VIDEOS, "rb"), *g;
    Video v;
    Usuario u;
    int total = 0, fas;

    printf("\n======== RELATORIO: VIDEOS E USUARIOS QUE OS FAVORITARAM ========\n\n");
    while (f != NULL && ler_registro(f, &v, sizeof v)) {
        printf("Video [%d] %s (%s, %d)\n", v.id, v.titulo, v.genero, v.ano);
        fas = 0;
        if ((g = fopen(ARQ_USUARIOS, "rb")) != NULL) {
            while (ler_registro(g, &u, sizeof u))
                if (indice_favorito(&u, v.id) >= 0) {
                    printf("    - %s <%s>\n", u.nome, u.email);
                    fas++;
                }
            fclose(g);
        }
        if (fas == 0) printf("    (ninguem favoritou ainda)\n\n");
        else          printf("    Total: %d usuario(s)\n\n", fas);
        total++;
    }
    if (f != NULL) fclose(f);
    printf("Total de videos: %d\n", total);
}

/* ------------------------ Dados de exemplo (demo) ------------------------ */

static void popular_exemplo(void) {
    static const Video videos[] = {
        { 1, "Interestelar",         "Ficcao Cientifica", 2014, 169 },
        { 2, "Matrix",               "Ficcao Cientifica", 1999, 136 },
        { 3, "O Poderoso Chefao",    "Drama",             1972, 175 },
        { 4, "Toy Story",            "Animacao",          1995,  81 },
        { 5, "Parasita",             "Suspense",          2019, 132 },
        { 6, "Vingadores: Ultimato", "Acao",              2019, 181 }
    };
    static const Usuario usuarios[] = {
        { 1, "Ana Souza",    "ana@email.com",   3, { 1, 2, 5 } },
        { 2, "Bruno Lima",   "bruno@email.com", 2, { 3, 4 } },
        { 3, "Carla Mendes", "carla@email.com", 4, { 1, 4, 5, 6 } }
    };

    if (arquivo_tem_registros(ARQ_VIDEOS) || arquivo_tem_registros(ARQ_USUARIOS))
        printf("Os arquivos ja possuem dados. Apague %s e %s para recarregar o exemplo.\n",
               ARQ_VIDEOS, ARQ_USUARIOS);
    else if (gravar(ARQ_VIDEOS, "wb", videos, sizeof videos[0], QTD(videos)) &&
             gravar(ARQ_USUARIOS, "wb", usuarios, sizeof usuarios[0], QTD(usuarios)))
        printf("Dados de exemplo carregados: %d videos e %d usuarios.\n",
               (int)QTD(videos), (int)QTD(usuarios));
    else
        printf("Erro na gravacao dos registros.\n");
}

/* ---------------------------------- Menu --------------------------------- */

int main(void) {
    int opcao;

    do {
        printf("\n==================== STREAMING DE VIDEOS ====================\n"
               " VIDEOS                          USUARIOS\n"
               "   1 - Cadastrar video             6 - Cadastrar usuario\n"
               "   2 - Buscar video por ID         7 - Buscar usuario por ID\n"
               "   3 - Listar todos os videos      8 - Listar todos os usuarios\n"
               "   4 - Atualizar video             9 - Atualizar usuario\n"
               "   5 - Remover video              10 - Remover usuario\n"
               " RELATORIOS\n"
               "  11 - Usuarios e seus videos favoritos\n"
               "  12 - Videos e os usuarios que os favoritaram\n"
               "  13 - Carregar dados de exemplo            0 - Sair\n"
               "=============================================================\n");
        opcao = pedir_int("Opcao: ", 0, 13, NULL);
        switch (opcao) {
        case 1:  cadastrar_video();              break;
        case 2:  buscar(VIDEO);                  break;
        case 3:  listar_todos(VIDEO);            break;
        case 4:  atualizar_video();              break;
        case 5:  remover(VIDEO);                 break;
        case 6:  cadastrar_usuario();            break;
        case 7:  buscar(USUARIO);                break;
        case 8:  listar_todos(USUARIO);          break;
        case 9:  atualizar_usuario();            break;
        case 10: remover(USUARIO);               break;
        case 11: relatorio_usuarios_favoritos(); break;
        case 12: relatorio_videos_fas();         break;
        case 13: popular_exemplo();              break;
        }
        if (opcao != 0) pausar();
    } while (opcao != 0);
    printf("Encerrando o programa. Ate logo!\n");
    return 0;
}