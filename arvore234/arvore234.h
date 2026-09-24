#ifndef ARVORE234_H
#define ARVORE234_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Estrutura que armazena os dados relevantes de cada paciente extraidos do CSV.
typedef struct {
    long long id;
    char data_registro[15];
    char cidade_estado[80];
    char sexo[5];
    char data_nascimento[15];
    char desfecho[35];
} RegistroPaciente;

// Definicoes de capacidade maxima de um no da arvore 
#define MAX_CHAVES 3   // maximo de chaves em um no (4-no = 3 chaves)
#define MAX_FILHOS 4   // maximo de filhos em um no

// Estrutura que define cada no da arvore, contendo ate 3 chaves, ate 4 filhos
// e os contadores que indicam quantas chaves/filhos estao efetivamente em uso.
typedef struct No234 {
    RegistroPaciente chaves[MAX_CHAVES];
    struct No234* filhos[MAX_FILHOS];
    int numChaves;
    int numFilhos;
    bool folha;
} No234;


No234* novoNo234(bool folha);

void   liberarArvore234(No234* r);

No234* buscar234(No234* r, long long id);

No234* inserirNo234(No234* r, RegistroPaciente dado);

No234* removerNo234(No234* r, long long id);

void   preOrdem234(No234* r);

void   emOrdem234(No234* r);

int    altura234(No234* r);

int    contarNos234(No234* r);

int    contarChaves234(No234* r);

void   pegar_coluna_por_indice(const char* linha, int coluna, char* resultado);

No234* carregarPlanilhaCSV234(const char* nomeArquivo, No234* raiz, long long* total_carregado);

void   imprimirCabecalhoTabela234(void);

void   imprimirEmOrdemTabela234(No234* raiz, long long* contador, long long limite);

#endif