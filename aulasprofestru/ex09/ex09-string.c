#include <stdio.h>
#include <string.h>

#define TAM 50

void lerStr( char str[], int tamMax);

int main() {
    char frase[TAM];
    printf("Digite uma frase: ");
    lerStr(frase, TAM);
    printf("\nA frase digitada foi: '%s' e te, tamanho %d.\n", frase, strlen(frase));
    return 0;,

}

void lerStr( char str[], int tamMax) {
    fgets(str, tamMax, stdin);
    int tam = strlen(str);
    if( tam > 0 && str[tam - 1] == '\n') {
        str[tam - 1] = '\0';
    }
}

int meuStrLen()