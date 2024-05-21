/*-------------------------------------------------------------------------
 *             Unifal - Universidade Federal de Alfenas
 *                BACHARELADO EM CIENCIA DA COMPUTACA
 * Trabalho..: Imagem ASCII
 * Disciplina: Processamento de Imagens
 * Professor.: Luiz Eduardo da Silva
 * Aluno.....: Gustavo Fidelis Camilo
 * Dara......: 21/04/2023
 *-------------------------------------------------------------------------*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "imagelib.h"

void msg(char *s)
{
    printf("\nTrabalho1 ASCII");
    printf("\n-------------------------------");
    printf("\nUso:  %s  nome-imagem[.pgm] <ncolunas> <nlinhas> \"Cadeia de caracteres\"\n\n", s);
    printf("    nome-imagem[.pgm] é o nome do arquivo da imagem.\n");
    printf("    <ncolunas> é o número de colunas que se deseja a saida.\n");
    printf("    <nlinhas>  é o número de linhas que se deseja a saida. \n");
    printf("    \"cadeia de caracteres\" é os caracteres que vão substituir os numeros. \n");
    exit(1);
}

/*-------------------------------------------------------------------------
 * main function
 *-------------------------------------------------------------------------*/
int main(int argc, char *argv[])
{
    int nc, nr, ml;
    int nlu; //numero de linhas do usuario
    int ncu; //numero de colunas do usuario
    char nameIn[100], nameOut[100], cmd[110];
    image In, Out;
    if (argc != 5)
        msg(argv[0]);
    img_name(argv[1], nameIn, nameOut, GRAY);
    //-- read image
    In = img_get(nameIn, &nr, &nc, &ml, GRAY);
    Out = img_alloc(nr, nc);
    //atof transforma a string em número, linha e coluna passados pelo usuario
    ncu = atof(argv[2]); 
    nlu = atof(argv[3]);

    //obter a string de caracteres, descobrir o tamanho e alocar em um vetor
    char *str = argv[4];
    int tamCarc = strlen(str);
    char carcs[tamCarc+1];
    for(int i = 0; i < tamCarc; i++ ){
        carcs[i] = str[i];
    }

    //Calcular o modulo para saber quanto pixels devo saltar
    int saltL = nr / nlu;
    int saltC = nc / ncu;
    printf("%d, %d", saltL, saltC);

    //percorrer a imagem alocando em um novo vetor
    
    int px, py;
    for (int i = 0; i < nlu; i++){
        for (int j = 0; j < ncu; j++){
            px = j * saltC;
            py = i * saltL;
            Out[i * ncu + j] = In[py * nc + px];
        }
    } 

    //normalizar para a nova intensidade 
    char result[ncu * nlu];
    for (int i = 0; i < nlu * ncu; i++){
        int novo_valor = pow(Out[i], 0.5) / pow(ml, 0.5) * tamCarc; 
        Out[i] = novo_valor; // troca o valor de cada posição de Out
        result[i] = carcs[Out[i]]; 
    }
    // gerar txt
    FILE *arq;
    arq = fopen("saida.txt", "w"); // abrir arquivo para escrita
    if(arq == NULL) {
        printf("Erro ao criar arquivo de saida.\n");
        exit(1);
    }
    for(int i = 0; i < nlu; i++) {
        for(int j = 0; j < ncu; j++) {
            fprintf(arq, "%c", result[i*ncu+j]); // escrever cada caractere em uma linha do arquivo
        }
        fprintf(arq, "\n"); // pular para a próxima linha do arquivo
    }
    fclose(arq); // fechar arquivo

    
    //-- save image
    img_put(Out, nameOut, nlu, ncu, tamCarc, GRAY);
    sprintf(cmd, "%s %s &", VIEW, nameOut);
    system(cmd);
    img_free(In);
    img_free(Out);
    return 0;
}
