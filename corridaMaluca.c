#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h> // Para toupper()

typedef struct {
    int XDIM;
    int YDIM;
    int MaxVal;
    char* pixels;
} Imagem;

void infoHeader(Imagem* imagem, char* filename) {
    FILE* ptrFile = fopen(filename, "rb");

    if (ptrFile == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    char linha[256];
    fgets(linha, sizeof(linha), ptrFile);
    if (linha[0] != 'P' || linha[1] != '2') {
        printf("Formato de arquivo PGM inválido.\n");
        exit(1);
    }

    // Lê a próxima linha que pode conter um comentário
    fgets(linha, sizeof(linha), ptrFile);

    // Agora estamos na linha que contém XDIM e YDIM
    sscanf(linha, "%d %d", &imagem->XDIM, &imagem->YDIM);

    // Lê a próxima linha para obter MaxVal (valor máximo dos pixels)
    fgets(linha, sizeof(linha), ptrFile);
    sscanf(linha, "%d", &imagem->MaxVal);

    fclose(ptrFile);
}

void loadPixelsText(Imagem* imagem, char* filename) {
    FILE* ptr = fopen(filename, "rb");
    if (ptr == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    // Pula as primeiras linhas
    char linha[256];
    fgets(linha, sizeof(linha), ptr); // P2
    fgets(linha, sizeof(linha), ptr); // Comentário
    fgets(linha, sizeof(linha), ptr); // Dimensões
    fgets(linha, sizeof(linha), ptr); // MaxVal

    // Alocando memória para os pixels
    imagem->pixels = (char*)malloc(imagem->XDIM * imagem->YDIM * sizeof(char));
    if (imagem->pixels == NULL) {
        printf("Erro ao alocar memória para pixels\n");
        exit(1);
    }

    // Lê os pixels um por um (cada um em uma linha)
    int pixValue;
    for (int i = 0; i < imagem->XDIM * imagem->YDIM; i++) {
        fscanf(ptr, "%d", &pixValue);
        imagem->pixels[i] = (char)pixValue; // Converte para char (0-255)
    }

    fclose(ptr);
}

void loadPixelsBinary(Imagem* imagem, char* filename) {
    FILE* ptr = fopen(filename, "rb");
    if (ptr == NULL) {
        printf("Erro ao abrir o arquivo\n");
        exit(1);
    }

    // Pula as primeiras linhas
    char linha[256];
    fgets(linha, sizeof(linha), ptr); // P2
    fgets(linha, sizeof(linha), ptr); // Comentário
    fgets(linha, sizeof(linha), ptr); // Dimensões
    fgets(linha, sizeof(linha), ptr); // MaxVal

    // Alocando memória para os pixels
    imagem->pixels = (char*)malloc(imagem->XDIM * imagem->YDIM * sizeof(char));
    if (imagem->pixels == NULL) {
        printf("Erro ao alocar memória para pixels\n");
        exit(1);
    }

    // Lê os pixels diretamente do arquivo binário
    fread(imagem->pixels, sizeof(char), imagem->XDIM * imagem->YDIM, ptr);

    fclose(ptr);
}

void processPixels(Imagem imagem) {
    // Exemplo de processamento simples: inverte os pixels
    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        imagem.pixels[i] = 255 - imagem.pixels[i];
    }
}

void printPixels(Imagem imagem) {
    printf("P2\n");
    printf("# Imagem processada\n");
    printf("%d %d\n", imagem.XDIM, imagem.YDIM);
    printf("%d\n", imagem.MaxVal);

    for (int i = 0; i < imagem.XDIM * imagem.YDIM; i++) {
        printf("%d\n", imagem.pixels[i]);
    }
}

int main() {
    char filename[100];

    scanf("%s", filename);

    Imagem imagem;
    infoHeader(&imagem, filename);

    if (imagem.MaxVal > 255) {
        printf("Apenas imagens com MaxVal <= 255 são suportadas.\n");
        return 1;
    }

    if (strstr(filename, ".pgm") != NULL) {
        loadPixelsText(&imagem, filename);
    } else {
        loadPixelsBinary(&imagem, filename);
    }

    processPixels(imagem);
    printPixels(imagem);

    free(imagem.pixels);

    return 0;
}
