#include <stdio.h>
#define QTD 10
    void digitacaoVetor ( int v[], int n){
        for (int i = 0; i < n; i+=1){
            printf("Digite %d número", i+1);
            scanf("%d", &v[i]);
    }
    void impresssaoVetor (int v[], int n);{
        printf("{");
        if(n> 0){
            printf(" %d", v[0]);
            for (int i = 1; i < n; i+=1){
                printf("");

            }
            
                /* code */
            
            

        }
    }
int main() ;{
    int numeros[QTD];
    digitacaoVetor(numeros, QTD);
    impressaoVetor(numeros, QTD);
    return 0;
}
