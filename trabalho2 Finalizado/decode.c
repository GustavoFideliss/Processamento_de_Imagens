/*-------------------------------------------------------------------------
 *             Unifal - Universidade Federal de Alfenas
 *                BACHARELADO EM CIENCIA DA COMPUTACA
 * Trabalho..: Esteganografia
 * Disciplina: Processamento de Imagens
 * Professor.: Luiz Eduardo da Silva
 * Aluno.....: Gustavo Fidelis Camilo
 * Dara......: 27/06/2023
 *-------------------------------------------------------------------------*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "imagelib.h"
#define DEBUG(x) x

/*
* seleciona apenas o ultimo bit, adicionando ele ao byte.
*/
unsigned char juntarBits(unsigned char byte, unsigned char bit) {
    byte <<= 1;
    byte |= bit;
    return byte;
}

void decode(image In)
{
    FILE *file;
    char name[100];
    int fsize = 0;
    unsigned char byte;

    int nr = In->nr;
    int nc = In->nc;
    int *px = In->px;

    char aux = 'a'; // auxiliar do name, para verificar se é "vazio"
    unsigned int R,G,B;
    // decode the name
    int countL  = 0     // para contar o numero de linhas
    , countC    = 0     // para contar o numero de colunas
    , countP    = 0     // para definir que existe 1 byte para ser codificado
    , aux2      = 0;    // para o resto das divisões

    int letras =0; // contar o numero de letras do nome

    //Alocando o caracter 'vazio' em todas as posições
    for(int i =0; i < 100; i++)
        name[i] = '\0';

    for(int i = 0; i < nr && aux != '\0'; i++){
        countL ++; 
        countC =  0;
        for(int j = 0; j < nc && aux != '\0'; j++){
            if(aux2 %  3 == 0){
                R = (px[i * nc + j] >> 16) & 0x1;
                byte = juntarBits(byte, R);
            }
            else if(aux2 % 3 == 1){
                G = (px[i * nc + j] >> 8) & 0x1;
                byte = juntarBits(byte, G);
            }
            else{
                B = (px[i * nc + j] & 0x1);
                byte = juntarBits(byte, B);
            }

            countP += 1; // limita o numero de bits
            if(countP == 8){ 
                aux = (char)byte;
                if(aux == '\0'){
                    countP =0;
                    if(countC < nc)
                        countL--;  //subtrai a linha caso ainda não tenha chegado no fim da linha
                    countC++;
                    aux2++;
                    break; 
                }
                else{
                    name[letras] = aux;
                    letras++;
                    byte = 0;  //resetando o byte
                    countP =0; // resetando o numero de bits
                }
            }
            aux2++;
            countC ++;
        }
    }

    printf("File name: %s\n", name);
    
    // decode file size

    int quatroBytes= 0; // limitar para 4 bytes, porem usando bits
    for(int i = countL; i < nr && quatroBytes < 32; i++){ 
        for(int j = countC; j < nc && quatroBytes < 32; j++){
            if(aux2 %  3 == 0){ 
                R = (px[i * nc + j] >> 16) & 0x1;
                byte = juntarBits(byte, R);
            }
            else if(aux2 % 3 == 1){
                G = (px[i * nc + j] >> 8) & 0x1;
                byte = juntarBits(byte, G);
            }
            else{
                B = (px[i * nc + j] & 0x1);
                byte = juntarBits(byte, B);
            }

            quatroBytes++;

            countP += 1; // limita o numero de bits
            if(countP == 8){
                fsize = (fsize << 8) | byte;  //obtenho o numero de bytes escondido 
                byte = 0;
                countP =0;
            }

            aux2++;
            countC ++;
        }
        if(quatroBytes < 32){
            countC  =  0;
            countL++;
        }
    }

    printf("File size: %d bytes\n", fsize);

    if(fsize > (nc*nr)/8){
        printf("O número de bytes codificado é maior que o arquivo");
        exit(10);
    }
    
    // decode file

    //aux2= countC %3; // apenas para pegar a posição que devo continuar, podendo ser o R, G ou B

    int aux1 =0;  // variavel para contar o numero de bytes codifidicados 
    
    file = fopen(name, "wb");
    if (!file)
    {
        printf("Cannot create file %s\n", name);
        exit(10);
    }
    
    while (fsize)
    {
        for(int i = countL; i < nr && aux1 < fsize; i++){ 
            for(int j = countC; j < nc&& aux1 <  fsize ; j++){
                if(aux2 %  3 == 0){ 
                    R = (px[i * nc + j] >> 16) & 0x1;
                    byte = juntarBits(byte, R);
                }
                else if(aux2 % 3 == 1){
                    G = (px[i * nc + j] >> 8) & 0x1;
                    byte = juntarBits(byte, G);
                }
                else {
                    B = (px[i * nc + j] & 0x1);
                    byte = juntarBits(byte, B);
                }

                aux2++;

                countP ++; // limita o numero de bits
                if(countP == 8){
                        fprintf(file,"%c",(char)byte);
                        countP = 0;
                        aux1++;
                }
                countC ++;
            }
            countC =0;
            countL ++;
        }
        fsize =0;  
    }
    fclose(file);
}

void msg(char *s)
{
    printf("\nSteganography - decode");
    printf("\n-------------------------------");
    printf("\nUSE.:  %s  <IMG.PPM>", s);
    printf("\nWhere:\n");
    printf("    <IMG.PPM>  Image file in PPM ASCII format\n\n");
    exit(20);
}

/*-----------------------------------------
  Main program
------------------------------------------*/
int main(int argc, char *argv[])
{
    char name[100];
    image In;
    char *p;
    if (argc < 2)
        msg(argv[0]);
    strcpy(name, argv[1]);
    p = strstr(name, ".ppm");
    if (p)
        *p = 0;
    strcat(name, ".ppm");
    In = img_get(name, COLOR);
    decode(In);
    img_free(In);
    return 0;
}
