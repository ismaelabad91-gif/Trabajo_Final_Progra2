#include<stdio.h>
#include<stdlib.h>

void limpiarBuffer();

int main(){
    int menu;

    printf("===============================\n");
    printf("\n\t S P O T I F Y\n");
    printf("\n===============================\n");
    return 0;
}

void limpiarBuffer(){
    int c;
    while((c = getchar()) != '\n' && c != EOF);
}