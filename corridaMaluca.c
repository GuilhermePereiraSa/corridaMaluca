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

typedef struct {
    int XDIM;
    int YDIM;
    int MaxVal;
    int MinVal;
    int nValue;
    char tipo[10];
    char *pixels;
}Imagem;

//functions 
void infoHeader(Imagem* image, char* argv[]);
void calculaMin(Imagem image, char* argv[]);
void loadPixelsTEXTO(Imagem image, char *argv[]);

void infoHeader(Imagem* image, char* argv[])
{
    FILE* ptrFile;
    ptrFile = fopen(argv[1], "rb");

    if (ptrFile == NULL) {
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

    // Agora estamos na linha que contém XDIM e YDIM
    // Vamos separar os valores XDIM e YDIM
    sscanf(linha, "%d %d", &image->XDIM, &image->YDIM);

    // Lê a próxima linha para obter MaxVal (valor máximo dos pixels)
    fgets(linha, sizeof(linha), ptrFile);
    sscanf(linha, "%d", &image->MaxVal);

    fclose(ptrFile);
}

void calculaMin(Imagem image, char* argv[])
{
    FILE *ptr;
    ptr = fopen(argv[1], "rb");

    fseek(ptr, 5, SEEK_CUR);

    int menorVal = 255;
    //inicializando com o maior possivel

    int pixel;
    for(int i=0; i< image.XDIM * image.YDIM; i++){
        fread(pixel, sizeof(pixel), 1, ptr);
        if(pixel < menorVal){
            menorVal = pixel;
        }
    }

    image.MinVal = menorVal;
}

void loadPixelsTEXTO(Imagem image, char *argv[])
{
    FILE *ptr;
    ptr = fopen(argv[1], "rb");

    fseek(ptr, 5, SEEK_CUR);

    image.pixels = (char*) malloc(image.XDIM * image.YDIM * sizeof(char));

    // Lê os pixels um por um (cada um em uma linha)
    int pixValue;

    for (int i = 0; i < image.XDIM * image.YDIM; i++) {
        fscanf(ptr, "%d", &pixValue);
        image.pixels[i] = (char) pixValue; // Converte para char (0-255)
    }

}

void loadPixelsBINARIO(Imagem image, char *argv[])
{
    FILE *ptr;
    ptr = fopen(argv[1], "rb");
    if(ptr == NULL){
        printf("erro");
        exit(0);
    }

    fseek(ptr, 5, SEEK_END);

    image.pixels = (char*) malloc(image.XDIM * image.YDIM * sizeof(char));

    fread(image.pixels, sizeof(image.pixels), (image.XDIM * image.YDIM), ptr);
}

int main(int argc, char* argv[]) 
{
    Imagem image;
    infoHeader(&image, argv);
    calculaMin(image, argv);
    if(image.nValue == 2){
        strcpy(image.tipo, "texto");
        loadPixelsTEXTO(image, argv);  
    }else{
        strcpy(image.tipo, "binario");
        loadPixelsBINARIO(image, argv);
    }
      
    saveImage(image, argv);
    
    free(image.pixels);
    return 0;
}