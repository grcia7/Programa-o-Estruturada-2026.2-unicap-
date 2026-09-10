#include <stdio.h>

#define QTD_MAX_VALORES 10

int main() {
  int num, soma = 0, qtd = 0;
  int numeros[QTD_MAX_VALORES];
  printf("Digite um valor inteiro positivo: ");
  scanf("%d", &num);
  while (num > 0) {
    numeros[qtd] = num;
    qtd += 1;
    if (qtd >= QTD_MAX_VALORES) {
      printf("Quantidade máxima de valores atingida!\n");
      break;
    }
    printf("Digite um valor inteiro positivo:");
    scanf("%d", &num);
}
printf("Os números digitados pelo usúario foram: ", numeros[10]);
return 0;
}