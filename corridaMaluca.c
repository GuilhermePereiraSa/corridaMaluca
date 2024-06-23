/*
<HEADER do PGM>
P<n>
# Comentario qualquer: normalmente o software que criou a imagem !
XDIM YDIM
MaxVal

    
//XDIM e YDIM - separados por linha ou espaço em branco
    //se não for na frente do XDIM, estará na próxima linha
//MaxVal - valor do maior pixel encontrado - entre 0 e 255

    
se nValue == 2: valores dos pixels em cada linha, abaixo do MaxVal

se nValue == 5: stream binária de tamanho XDIM * YDIM, que contém todos os bytes da imagem


y = c * log(1+y) com c = 255 / log(1+max), onde:
y é todo pixel da nova imagem; x é todo pixel da imagem original e max é o pixel de maior
valor da imagem original.

   
saída em formato ASCII, logo, é necessário outro ptr para criação de um arquivo em txt
   na segunda linha: # CREATOR: Image Generator SCC-222 – Lab ICC I
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct{
    int XDIM;
    int YDIM;
    int MaxVal;
    int MinVal;
    int nValue;
}Imagem;

//functions 
void infoHeader(Imagem *image, char *argv[]);

void infoHeader(Imagem *image,char *argv[])
{   
    FILE *ptrFile;
    ptrFile = fopen(argv[1], "r");

    if(ptrFile == NULL){
        printf("Erro ao abrir o arquivo\n");
        exit(0);
    }

    char linha[256];
    fseek(ptrFile, 0, SEEK_SET);
    fgets(linha, sizeof(linha), ptrFile);
    //P<n>
    image->nValue = atoi(linha[3]);

    fgets(linha, sizeof(linha), ptrFile);
    //comentario

    fgets(linha, sizeof(linha), ptrFile);
    //na linha de dimensoes é necessário verificar se há algum espaço

    /*
    se tiver o espaço, então você aponta para o strlength + \0
    */
    for(int i=0; i < strlen(linha); i++){
        if(linha[i] == 32){

        }
    }

    fclose(ptrFile);
}



int main(int argc, char *argv[]){
    Imagem image;
    infoHeader(&image, argv);  
    

    return 0;
}