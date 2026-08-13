#include <stdio.h>

#define VALOR_INICIAL 2
#define VALOR_MAXIMO 50
#define VALOR_INCREMENTO 2


int main () {
    for (int i = VALOR_INICIAL; i < VALOR_MAXIMO ; i+= VALOR_INCREMENTO) {
        printf("%d", i);
        if (i <VALOR_MAXIMO) {
                printf(" , ");
        }
    }
    printf("\n");
    return 0;
}