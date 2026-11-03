#ifndef ARVORE234_H
#define ARVORE234_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/*==============================================================
 *  CHAVE COMPOSTA (cidade_estado, data_registro, id)
 *==============================================================*/
#define TAM_CIDADE 80
#define TAM_DATA   15

typedef struct {
    char      cidade_estado[TAM_CIDADE];
    char      data_registro[TAM_DATA];
    long long id;
} Chave234;

int compararChaves(const Chave234* a, const Chave234* b);

typedef struct {
    Chave234 chave;
    char     sexo[5];
    char     data_nascimento[TAM_DATA];
    char     desfecho[40];
} RegistroPaciente;

/*==============================================================
 *  ÁRVORE 2-3-4
 *==============================================================*/
#define MAX_CHAVES 3
#define MAX_FILHOS 4

typedef struct No234 {
    RegistroPaciente chaves[MAX_CHAVES];
    struct No234*    filhos[MAX_FILHOS];
    int numChaves;
    int numFilhos;
    bool folha;
} No234;

/* Contadores de métricas */
typedef struct {
    long long comparacoes;
    long long splits;
    long long insercoes_ok;
    long long insercoes_dup;
} Metricas234;

extern Metricas234 g_metricas234;
void resetMetricas234(void);

/*==============================================================
 *  API PÚBLICA
 *==============================================================*/
No234* novoNo234(bool folha);
void   liberarArvore234(No234* r);

No234* buscar234(No234* r, const Chave234* chave);

bool   inserirNo234(No234** raizRef, RegistroPaciente dado);
bool   removerNo234(No234** raizRef, const Chave234* chave);

void   preOrdem234(No234* r);
void   emOrdem234(No234* r);

int    altura234(No234* r);
int    contarNos234(No234* r);
int    contarChaves234(No234* r);
size_t estimarMemoria234(No234* r);

/*==============================================================
 *  VALIDAÇÃO AUTOMATIZADA
 *==============================================================*/
typedef struct {
    bool ok;
    int  alturaFolhas;
    int  erros;
    char mensagem[256];
} ResultadoValidacao234;

ResultadoValidacao234 validar234(No234* r);

/*==============================================================
 *  LEITURA DO CSV
 *==============================================================*/
void pegar_coluna_por_indice(const char* linha, int coluna, char* resultado);

/* Lê o CSV e preenche um vetor dinâmico. Se limite > 0, para após
 * `limite` registros válidos; se limite <= 0, lê todos. */
long long lerCSV234(const char* nomeArquivo, RegistroPaciente** registros,
                    long long limite);

void imprimirCabecalhoTabela234(void);
void imprimirEmOrdemTabela234(No234* raiz, long long* contador, long long limite);

#endif