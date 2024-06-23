/*
 Corrida Maluca - é necessária a visualização de um velocímetro de uma imagem em formato pgm,
 que está muito "saturada e escura".
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
    int XDIM;
    int YDIM;
    int MaxVal;
    int MinVal;
    int nValue;
    char tipo[10];
    char* pixels;
    // tipo e MinVal são desnecessários
}Imagem;

//functions 
void infoHeader(Imagem* imagem, char* argv[]);
void calculateMin(Imagem imagem, char* argv[]);
void loadPixelsText(Imagem imagem, char* argv[]);
void loadPixelsBinary(Imagem imagem, char* argv[]);
void createNewText(Imagem imagem, Imagem novaImagem, char* argv[]);
void createNewBinary(Imagem imagem, Imagem novaImagem, char* argv[]);
FILE* callNewFile(char* argv[]);
int htoi(const char* hex_string);


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

void calculateMin(Imagem imagem, char* argv[])
{
    FILE* ptr;
    ptr = fopen(argv[1], "rb");

    fseek(ptr, 5, SEEK_CUR);

    int menorVal = 255;
    //inicializando com o maior possivel

    int pixel;
    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        fread(pixel, sizeof(pixel), 1, ptr);
        if (pixel < menorVal) {
            menorVal = pixel;
        }
    }

    imagem.MinVal = menorVal;
}

void loadPixelsText(Imagem imagem, char* argv[])
{
    FILE* ptr;
    ptr = fopen(argv[1], "rb");

    fseek(ptr, 5, SEEK_CUR);

    imagem.pixels = (char*)malloc(imagem.XDIM * imagem.YDIM * sizeof(char));
    if (imagem.pixels == NULL) {
        printf("erro");
        exit(0);
    }

    // Lê os pixels um por um (cada um em uma linha)
    int pixValue;

    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        fscanf(ptr, "%d", &pixValue);
        imagem.pixels[i] = (char)pixValue; // Converte para char (0-255)
    }

}

void loadPixelsBinary(Imagem imagem, char* argv[])
{
    FILE* ptr;
    ptr = fopen(argv[1], "rb");
    if (ptr == NULL) {
        printf("erro");
        exit(0);
    }

    fseek(ptr, 5, SEEK_END);

    imagem.pixels = (char*)malloc(imagem.XDIM * imagem.YDIM * sizeof(char));
    if (imagem.pixels == NULL) {
        printf("erro");
        exit(0);
    }

    fread(imagem.pixels, sizeof(imagem.pixels), (imagem.XDIM * imagem.YDIM), ptr);

}

void createNewText(Imagem imagem, Imagem novaImagem, char* argv[])
{
    FILE* ptrFILE = callNewFile(argv);

    fprintf(ptrFILE, "P<2>");
    fprintf(ptrFILE, "# CREATOR: Image Generator SCC-222 - Lab ICC I");
    fprintf(ptrFILE, "%d %d", imagem.XDIM, imagem.YDIM);
    fprintf(ptrFILE, "%d", imagem.MaxVal);

    /*
     y = c * log(1+y) com c = 255 / log(1+max), onde:
     c é constante;
     y é todo pixel da nova imagem; x é todo pixel da imagem original e max é o pixel de maior
     valor da imagem original.
    */

    double c = 255 / log10(1 + imagem.MaxVal);

    // calculando constante c e armazenando na heap os pixels da novaImagem

    novaImagem.pixels = malloc(imagem.XDIM * imagem.YDIM * sizeof(char));
    if (novaImagem.pixels == NULL) {
        printf("erro");
        exit(0);
    }

    // cada pixel da novaImagem terá o seguinte cálculo
    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        novaImagem.pixels[i] = c * log10(1 + novaImagem.pixels[i]);
        fprintf(ptrFILE, "%s", novaImagem.pixels[i]);
    }

}

void createNewBinary(Imagem imagem, Imagem novaImagem, char* argv[])
{
    FILE* ptrFILE = callNewFile(argv);

    fprintf(ptrFILE, "P<2>");
    fprintf(ptrFILE, "# CREATOR: Image Generator SCC-222 - Lab ICC I");
    fprintf(ptrFILE, "%d %d", imagem.XDIM, imagem.YDIM);
    fprintf(ptrFILE, "%d", imagem.MaxVal);

    novaImagem.pixels = malloc(imagem.XDIM * imagem.YDIM * sizeof(char));
    if (novaImagem.pixels == NULL) {
        printf("erro");
        exit(0);
    }

    double c = 255 / log10(1 + imagem.MaxVal);

    /*

    \
    0
    0
    \0

    proxima:
    \
    0
    5
    \0

    */


    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        // apenas os dois digitos
        char token[3] = { 0 };

        //exemplo mais acima;
        char hex[4] = { 0 };

        fread(hex, sizeof(char), 4, ptrFILE);

        // hex em [1] e em [2] são 
        token[0] = hex[1];
        token[1] = hex[2];

        int y = htoi(token);
        y = c * log10(1 + y);
        //todo pixel calculado

        //colocando cada pixel no vetor de novaImagem
        novaImagem.pixels[i] = y;

        //colocando no arquivo;
        fwrite(y, sizeof(y), 1, ptrFILE);

    }

}

// Função para converter string hexadecimal para inteiro
//tirada da net 🙃
// Função para converter string hexadecimal para inteiro
int htoi(const char* hex_string) {
    int result = 0;
    int len = strlen(hex_string);
    int base = 1;

    // Converter cada caractere hexadecimal para inteiro
    for (int i = len - 1; i >= 0; i--) {
        int digit_value;
        if (isdigit(hex_string[i])) {
            digit_value = hex_string[i] - '0';
        }
        else {
            digit_value = toupper(hex_string[i]) - 'A' + 10;
        }
        result += digit_value * base;
        base *= 16;
    }

    return result;
}


FILE* callNewFile(char* argv[])
{
    FILE* ptrFILE;
    char* novoNome;
    strcpy(novoNome, argv[1]);

    ptrFILE = fopen(novoNome, "w+");

    return ptrFILE;
}

int main(int argc, char* argv[])
{
    Imagem imagem;
    infoHeader(&imagem, argv);
    calculateMin(imagem, argv);
    Imagem novaImagem;

    if (imagem.nValue == 2) {
        strcpy(imagem.tipo, "Texto");
        loadPixelsText(imagem, argv);
        createNewText(imagem, novaImagem, argv);
    }
    else {
        strcpy(imagem.tipo, "Binario");
        loadPixelsBinary(imagem, argv);
        createNewBinary(imagem, novaImagem, argv);
    }

    free(imagem.pixels);
    free(novaImagem.pixels);
    //pode ainda fazê-las apontar para NULL
    return 0;
}