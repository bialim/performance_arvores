#ifndef ARVORE_AVL_H
#define ARVORE_AVL_H

#include <stdbool.h>
#include <stddef.h>

typedef struct {
    long long id;
    char data_registro[15];
    char cidade_estado[80];
    char sexo[5];
    char data_nascimento[15];
    char desfecho[40];
} RegistroPaciente;

typedef struct {
    char cidade_estado[80];
    char data_registro[15];
    long long id;
} ChaveAVL;

typedef struct IndiceIdsAVL IndiceIdsAVL;

typedef struct NoAVL {
    RegistroPaciente dado;
    int altura;
    struct NoAVL* esquerda;
    struct NoAVL* direita;
    IndiceIdsAVL* indiceIds;
} NoAVL;

typedef struct {
    long long comparacoes;
    long long rotacoes;
    long long insercoes_ok;
    long long insercoes_dup;
} MetricasAVL;

typedef struct {
    bool ok;
    int altura;
    int erros;
    char mensagem[256];
} ResultadoValidacaoAVL;

extern MetricasAVL g_metricasAVL;
void resetMetricasAVL(void);

int altura(NoAVL* no);
NoAVL* inserirAVL(NoAVL* raiz, RegistroPaciente dado);
NoAVL* buscarAVL(NoAVL* raiz, const ChaveAVL* chave);
bool removerAVL(NoAVL** raizRef, const ChaveAVL* chave);
void liberarArvoreAVL(NoAVL* raiz);
int contarNosAVL(NoAVL* raiz);
size_t estimarMemoriaAVL(NoAVL* raiz);
ResultadoValidacaoAVL validarAVL(NoAVL* raiz);

void pegar_coluna_por_indice(const char* linha, int coluna, char* resultado, size_t capacidade);
long long lerCSVAVL(const char* nomeArquivo, RegistroPaciente** registros, long long limite);
void imprimirCabecalhoTabela(void);
void imprimirEmOrdemTabela(NoAVL* raiz, long long* contador, long long limite);

#endif