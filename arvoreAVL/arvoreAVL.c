#include "arvoreAVL.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

MetricasAVL g_metricasAVL = {0, 0, 0, 0};

void resetMetricasAVL(void) {
    g_metricasAVL = (MetricasAVL){0, 0, 0, 0};
}

struct IndiceIdsAVL {
    long long* ids;
    unsigned char* estados;
    size_t capacidade;
    size_t ocupados;
};

static size_t hashIdAVL(long long id) {
    uint64_t valor = (uint64_t)id + UINT64_C(0x9e3779b97f4a7c15);
    valor = (valor ^ (valor >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    valor = (valor ^ (valor >> 27)) * UINT64_C(0x94d049bb133111eb);
    valor ^= valor >> 31;
    return (size_t)valor;
}

static IndiceIdsAVL* novoIndiceIdsAVL(size_t capacidade) {
    IndiceIdsAVL* indice = malloc(sizeof(*indice));
    if (indice == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    indice->ids = calloc(capacidade, sizeof(*indice->ids));
    indice->estados = calloc(capacidade, sizeof(*indice->estados));
    if (indice->ids == NULL || indice->estados == NULL) {
        perror("calloc");
        free(indice->ids);
        free(indice->estados);
        free(indice);
        exit(EXIT_FAILURE);
    }
    indice->capacidade = capacidade;
    indice->ocupados = 0;
    return indice;
}

static bool contemIdAVL(const IndiceIdsAVL* indice, long long id) {
    size_t posicao = hashIdAVL(id) & (indice->capacidade - 1);
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[posicao] == 0) return false;
        if (indice->estados[posicao] == 1 && indice->ids[posicao] == id) return true;
        posicao = (posicao + 1) & (indice->capacidade - 1);
    }
    return false;
}

static void redimensionarIndiceIdsAVL(IndiceIdsAVL* indice) {
    size_t capacidade = indice->capacidade * 2;
    long long* ids = calloc(capacidade, sizeof(*ids));
    unsigned char* estados = calloc(capacidade, sizeof(*estados));
    if (ids == NULL || estados == NULL) {
        perror("calloc");
        free(ids);
        free(estados);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[i] != 1) continue;
        size_t posicao = hashIdAVL(indice->ids[i]) & (capacidade - 1);
        while (estados[posicao] == 1)
            posicao = (posicao + 1) & (capacidade - 1);
        ids[posicao] = indice->ids[i];
        estados[posicao] = 1;
    }
    free(indice->ids);
    free(indice->estados);
    indice->ids = ids;
    indice->estados = estados;
    indice->capacidade = capacidade;
    indice->ocupados = 0;
    for (size_t i = 0; i < capacidade; i++)
        if (estados[i] == 1) indice->ocupados++;
}

static void inserirIdAVL(IndiceIdsAVL* indice, long long id) {
    if ((indice->ocupados + 1) * 10 >= indice->capacidade * 7)
        redimensionarIndiceIdsAVL(indice);
    size_t posicao = hashIdAVL(id) & (indice->capacidade - 1);
    size_t removido = SIZE_MAX;
    while (indice->estados[posicao] != 0) {
        if (indice->estados[posicao] == 2 && removido == SIZE_MAX)
            removido = posicao;
        posicao = (posicao + 1) & (indice->capacidade - 1);
    }
    if (removido != SIZE_MAX) posicao = removido;
    else indice->ocupados++;
    indice->ids[posicao] = id;
    indice->estados[posicao] = 1;
}

static void removerIdAVL(IndiceIdsAVL* indice, long long id) {
    size_t posicao = hashIdAVL(id) & (indice->capacidade - 1);
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[posicao] == 0) return;
        if (indice->estados[posicao] == 1 && indice->ids[posicao] == id) {
            indice->estados[posicao] = 2;
            return;
        }
        posicao = (posicao + 1) & (indice->capacidade - 1);
    }
}

static void liberarIndiceIdsAVL(IndiceIdsAVL* indice) {
    if (indice == NULL) return;
    free(indice->ids);
    free(indice->estados);
    free(indice);
}

static ChaveAVL chaveDoRegistroAVL(const RegistroPaciente* registro) {
    ChaveAVL chave;
    memcpy(chave.cidade_estado, registro->cidade_estado, sizeof(chave.cidade_estado));
    memcpy(chave.data_registro, registro->data_registro, sizeof(chave.data_registro));
    chave.id = registro->id;
    return chave;
}

static int compararChavesAVL(const ChaveAVL* a, const ChaveAVL* b) {
    int comparacao = strcmp(a->cidade_estado, b->cidade_estado);
    if (comparacao != 0) return comparacao;
    comparacao = strcmp(a->data_registro, b->data_registro);
    if (comparacao != 0) return comparacao;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return 1;
    return 0;
}

static void indexarNosAVL(NoAVL* raiz, IndiceIdsAVL* indice) {
    if (raiz == NULL) return;
    inserirIdAVL(indice, raiz->dado.id);
    indexarNosAVL(raiz->esquerda, indice);
    indexarNosAVL(raiz->direita, indice);
}

static void garantirIndiceIdsAVL(NoAVL* raiz) {
    if (raiz == NULL || raiz->indiceIds != NULL) return;
    IndiceIdsAVL* indice = novoIndiceIdsAVL(16);
    indexarNosAVL(raiz, indice);
    raiz->indiceIds = indice;
}


// Retorna a altura de um no de forma segura (se o no for nulo, retorna 0).
int altura(NoAVL *N) {
    if (N == NULL) return 0;
    return N->altura;
}

// Retorna o maior entre dois numeros inteiros (usado para calcular a altura da arvore).
int max(int a, int b) {
    return (a > b) ? a : b;
}

// Aloca dinamicamente na memoria um novo no para a arvore AVL com os dados fornecidos.
NoAVL* novoNo(RegistroPaciente dado) {
    NoAVL* no = (NoAVL*)malloc(sizeof(NoAVL));
    if (no == NULL) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    no->dado = dado;
    no->esquerda = NULL;
    no->direita = NULL;
    no->indiceIds = NULL;
    no->altura = 1;
    return no;
}

// Executa uma rotacao simples a direita para corrigir o desbalanceamento da arvore.
NoAVL* rotacaoDireita(NoAVL *y) {
    NoAVL *x = y->esquerda;
    NoAVL *T2 = x->direita;
    x->direita = y;
    y->esquerda = T2;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    g_metricasAVL.rotacoes++;
    return x;
}

// Executa uma rotacao simples a esquerda para corrigir o desbalanceamento da arvore.
NoAVL* rotacaoEsquerda(NoAVL *x) {
    NoAVL *y = x->direita;
    NoAVL *T2 = y->esquerda;
    y->esquerda = x;
    x->direita = T2;
    x->altura = max(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = max(altura(y->esquerda), altura(y->direita)) + 1;
    g_metricasAVL.rotacoes++;
    return y;
}

// Calcula e retorna o fator de balanceamento de um no (diferenca entre a altura da esquerda e da direita).
int obterBalanceamento(NoAVL *N) {
    if (N == NULL) return 0;
    return altura(N->esquerda) - altura(N->direita);
}

// Insere um novo registo de paciente na arvore AVL de forma ordenada por ID e aplica as rotacoes se necessario.
static NoAVL* inserirRecAVL(NoAVL* no, RegistroPaciente dado,
                            const ChaveAVL* chave, bool* inserido) {
    if (no == NULL) {
        *inserido = true;
        return novoNo(dado);
    }
    g_metricasAVL.comparacoes++;
    ChaveAVL chaveNo = chaveDoRegistroAVL(&no->dado);
    int comparacao = compararChavesAVL(chave, &chaveNo);
    if (comparacao < 0) no->esquerda = inserirRecAVL(no->esquerda, dado, chave, inserido);
    else if (comparacao > 0) no->direita = inserirRecAVL(no->direita, dado, chave, inserido);
    else return no;

    if (!*inserido) return no;

    no->altura = 1 + max(altura(no->esquerda), altura(no->direita));
    int balanceamento = obterBalanceamento(no);

    if (balanceamento > 1) {
        ChaveAVL chaveEsquerda = chaveDoRegistroAVL(&no->esquerda->dado);
        if (compararChavesAVL(chave, &chaveEsquerda) < 0) return rotacaoDireita(no);
        no->esquerda = rotacaoEsquerda(no->esquerda);
        return rotacaoDireita(no);
    }
    if (balanceamento < -1) {
        ChaveAVL chaveDireita = chaveDoRegistroAVL(&no->direita->dado);
        if (compararChavesAVL(chave, &chaveDireita) > 0) return rotacaoEsquerda(no);
        no->direita = rotacaoDireita(no->direita);
        return rotacaoEsquerda(no);
    }
    return no;
}

NoAVL* inserirAVL(NoAVL* no, RegistroPaciente dado) {
    if (no == NULL) {
        NoAVL* raiz = novoNo(dado);
        raiz->indiceIds = novoIndiceIdsAVL(16);
        inserirIdAVL(raiz->indiceIds, dado.id);
        g_metricasAVL.insercoes_ok++;
        return raiz;
    }
    garantirIndiceIdsAVL(no);
    IndiceIdsAVL* indice = no->indiceIds;
    if (contemIdAVL(indice, dado.id)) {
        g_metricasAVL.insercoes_dup++;
        return no;
    }
    bool inserido = false;
    ChaveAVL chave = chaveDoRegistroAVL(&dado);
    NoAVL* raiz = inserirRecAVL(no, dado, &chave, &inserido);
    if (raiz != no) no->indiceIds = NULL;
    raiz->indiceIds = indice;
    if (inserido) {
        inserirIdAVL(indice, dado.id);
        g_metricasAVL.insercoes_ok++;
    } else g_metricasAVL.insercoes_dup++;
    return raiz;
}

NoAVL* buscarAVL(NoAVL* raiz, const ChaveAVL* chave) {
    while (raiz != NULL) {
        g_metricasAVL.comparacoes++;
        ChaveAVL chaveNo = chaveDoRegistroAVL(&raiz->dado);
        int comparacao = compararChavesAVL(chave, &chaveNo);
        if (comparacao == 0) return raiz;
        raiz = comparacao < 0 ? raiz->esquerda : raiz->direita;
    }
    return NULL;
}

static NoAVL* menorNoAVL(NoAVL* raiz) {
    while (raiz->esquerda != NULL) raiz = raiz->esquerda;
    return raiz;
}

static NoAVL* removerRecAVL(NoAVL* raiz, const ChaveAVL* chave, bool* removido) {
    if (raiz == NULL) return NULL;

    g_metricasAVL.comparacoes++;
    ChaveAVL chaveNo = chaveDoRegistroAVL(&raiz->dado);
    int comparacao = compararChavesAVL(chave, &chaveNo);
    if (comparacao < 0) {
        raiz->esquerda = removerRecAVL(raiz->esquerda, chave, removido);
    } else if (comparacao > 0) {
        raiz->direita = removerRecAVL(raiz->direita, chave, removido);
    } else {
        *removido = true;
        if (raiz->esquerda == NULL || raiz->direita == NULL) {
            NoAVL* filho = raiz->esquerda != NULL ? raiz->esquerda : raiz->direita;
            free(raiz);
            return filho;
        }
        NoAVL* sucessor = menorNoAVL(raiz->direita);
        raiz->dado = sucessor->dado;
        bool removidoSucessor = false;
        ChaveAVL chaveSucessor = chaveDoRegistroAVL(&sucessor->dado);
        raiz->direita = removerRecAVL(raiz->direita, &chaveSucessor,
                          &removidoSucessor);
    }

    raiz->altura = 1 + max(altura(raiz->esquerda), altura(raiz->direita));
    int balanceamento = obterBalanceamento(raiz);
    if (balanceamento > 1 && obterBalanceamento(raiz->esquerda) >= 0)
        return rotacaoDireita(raiz);
    if (balanceamento > 1 && obterBalanceamento(raiz->esquerda) < 0) {
        raiz->esquerda = rotacaoEsquerda(raiz->esquerda);
        return rotacaoDireita(raiz);
    }
    if (balanceamento < -1 && obterBalanceamento(raiz->direita) <= 0)
        return rotacaoEsquerda(raiz);
    if (balanceamento < -1 && obterBalanceamento(raiz->direita) > 0) {
        raiz->direita = rotacaoDireita(raiz->direita);
        return rotacaoEsquerda(raiz);
    }
    return raiz;
}

bool removerAVL(NoAVL** raizRef, const ChaveAVL* chave) {
    if (*raizRef == NULL || chave == NULL) return false;
    garantirIndiceIdsAVL(*raizRef);
    if (buscarAVL(*raizRef, chave) == NULL) return false;
    IndiceIdsAVL* indice = (*raizRef)->indiceIds;
    (*raizRef)->indiceIds = NULL;
    bool removido = false;
    *raizRef = removerRecAVL(*raizRef, chave, &removido);
    if (removido) removerIdAVL(indice, chave->id);
    if (*raizRef != NULL) (*raizRef)->indiceIds = indice;
    else liberarIndiceIdsAVL(indice);
    return removido;
}

static void liberarNosAVL(NoAVL* raiz) {
    if (raiz == NULL) return;
    liberarNosAVL(raiz->esquerda);
    liberarNosAVL(raiz->direita);
    free(raiz);
}

void liberarArvoreAVL(NoAVL* raiz) {
    if (raiz == NULL) return;
    liberarIndiceIdsAVL(raiz->indiceIds);
    liberarNosAVL(raiz);
}

int contarNosAVL(NoAVL* raiz) {
    if (raiz == NULL) return 0;
    return 1 + contarNosAVL(raiz->esquerda) + contarNosAVL(raiz->direita);
}

size_t estimarMemoriaAVL(NoAVL* raiz) {
    if (raiz == NULL) return 0;
    size_t memoria = sizeof(NoAVL);
    if (raiz->indiceIds != NULL)
        memoria += sizeof(*raiz->indiceIds) + raiz->indiceIds->capacidade *
                   (sizeof(*raiz->indiceIds->ids) + sizeof(*raiz->indiceIds->estados));
    return memoria + estimarMemoriaAVL(raiz->esquerda) + estimarMemoriaAVL(raiz->direita);
}

static int validarRecAVL(NoAVL* raiz, const ChaveAVL* minimo,
                         const ChaveAVL* maximo, ResultadoValidacaoAVL* resultado) {
    if (raiz == NULL) return 0;
    ChaveAVL chave = chaveDoRegistroAVL(&raiz->dado);
    if ((minimo != NULL && compararChavesAVL(&chave, minimo) <= 0) ||
        (maximo != NULL && compararChavesAVL(&chave, maximo) >= 0)) {
        resultado->erros++;
        snprintf(resultado->mensagem, sizeof(resultado->mensagem), "Ordem da chave composta violada no ID %lld", raiz->dado.id);
    }

    int alturaEsquerda = validarRecAVL(raiz->esquerda, minimo, &chave, resultado);
    int alturaDireita = validarRecAVL(raiz->direita, &chave, maximo, resultado);
    int alturaCalculada = 1 + max(alturaEsquerda, alturaDireita);
    int fator = alturaEsquerda - alturaDireita;
    if (raiz->altura != alturaCalculada) {
        resultado->erros++;
        snprintf(resultado->mensagem, sizeof(resultado->mensagem), "Altura incorreta no ID %lld", raiz->dado.id);
    }
    if (fator < -1 || fator > 1) {
        resultado->erros++;
        snprintf(resultado->mensagem, sizeof(resultado->mensagem), "Fator de balanceamento invalido no ID %lld", raiz->dado.id);
    }
    return alturaCalculada;
}

ResultadoValidacaoAVL validarAVL(NoAVL* raiz) {
    ResultadoValidacaoAVL resultado = {true, 0, 0, ""};
    resultado.altura = validarRecAVL(raiz, NULL, NULL, &resultado);
    resultado.ok = resultado.erros == 0;
    return resultado;
}

// Extrai o texto de uma coluna especifica de uma linha do CSV com base no separador ponto e virgula (;), ignorando aspas.
void pegar_coluna_por_indice(const char *linha, int coluna_desejada, char *resultado, size_t capacidade) {
    int col_atual = 0;
    int i = 0, j = 0;
    if (capacidade == 0) return;
    resultado[0] = '\0';

    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        if (col_atual == coluna_desejada) {
            while (linha[i] != ';' && linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
                if (linha[i] != '"' && (size_t)j + 1 < capacidade) {
                    resultado[j++] = linha[i];
                }
                i++;
            }
            resultado[j] = '\0';
            return;
        }
        if (linha[i] == ';') {
            col_atual++;
        }
        i++;
    }
}

// Imprime na consola o cabecalho formatado da tabela de dados dos pacientes.
void imprimirCabecalhoTabela(void) {
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
    printf("| ID Paciente    | Registro     | Nascimento   | Cidade e Estado           | Sexo | Desfecho             |\n");
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
}

// Percorre a arvore em ordem (In-Order) e imprime os dados dos pacientes formatados em forma de tabela na consola.
void imprimirEmOrdemTabela(NoAVL* raiz, long long* contador_linhas, long long limite) {
    if (raiz == NULL || *contador_linhas >= limite) return;
    imprimirEmOrdemTabela(raiz->esquerda, contador_linhas, limite);
    if (*contador_linhas >= limite) return;
    if (*contador_linhas % 40 == 0) imprimirCabecalhoTabela();
    printf("| %-14lld | %-12.12s | %-12.12s | %-25.25s | %-4.4s | %-20.20s |\n",
           raiz->dado.id, raiz->dado.data_registro, raiz->dado.data_nascimento,
           raiz->dado.cidade_estado, raiz->dado.sexo, raiz->dado.desfecho);
    (*contador_linhas)++;
    imprimirEmOrdemTabela(raiz->direita, contador_linhas, limite);
}

long long lerCSVAVL(const char* nomeArquivo, RegistroPaciente** registros, long long limite) {
    *registros = NULL;
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (arquivo == NULL) {
        char alternativo[512];
        int tamanho = snprintf(alternativo, sizeof(alternativo), "../%s", nomeArquivo);
        if (tamanho > 0 && (size_t)tamanho < sizeof(alternativo)) arquivo = fopen(alternativo, "r");
    }
    if (arquivo == NULL) {
        printf("Erro: nao foi possivel abrir o ficheiro '%s'.\n", nomeArquivo);
        return 0;
    }

    char linha[40000];
    if (fgets(linha, sizeof(linha), arquivo) == NULL) {
        fclose(arquivo);
        return 0;
    }
    long long capacidade = 1024, total = 0;
    RegistroPaciente* dados = malloc((size_t)capacidade * sizeof(*dados));
    if (dados == NULL) {
        fclose(arquivo);
        return 0;
    }

    while (fgets(linha, sizeof(linha), arquivo) != NULL && (limite <= 0 || total < limite)) {
        RegistroPaciente registro = {0};
        char buffer[256], estado[10], cidade[90];
        pegar_coluna_por_indice(linha, 0, buffer, sizeof(buffer));
        registro.id = atoll(buffer);
        if (registro.id <= 0) continue;
        pegar_coluna_por_indice(linha, 1, registro.data_registro, sizeof(registro.data_registro));
        char* corte = strchr(registro.data_registro, 'T');
        if (corte != NULL) *corte = '\0';
        pegar_coluna_por_indice(linha, 5, estado, sizeof(estado));
        pegar_coluna_por_indice(linha, 8, cidade, sizeof(cidade));
        snprintf(registro.cidade_estado, sizeof(registro.cidade_estado), "%s-%s", cidade, estado);
        pegar_coluna_por_indice(linha, 10, registro.sexo, sizeof(registro.sexo));
        pegar_coluna_por_indice(linha, 11, registro.data_nascimento, sizeof(registro.data_nascimento));
        corte = strchr(registro.data_nascimento, 'T');
        if (corte != NULL) *corte = '\0';

        char evolucao[10], textoEvolucao[25], dataEvolucao[25];
        pegar_coluna_por_indice(linha, 110, evolucao, sizeof(evolucao));
        if (strcmp(evolucao, "1") == 0 || strcmp(evolucao, "1.0") == 0) strcpy(textoEvolucao, "Cura");
        else if (strcmp(evolucao, "2") == 0 || strcmp(evolucao, "2.0") == 0) strcpy(textoEvolucao, "Obito");
        else if (strcmp(evolucao, "3") == 0 || strcmp(evolucao, "3.0") == 0) strcpy(textoEvolucao, "Outras causas");
        else strcpy(textoEvolucao, "Desconhecido");
        pegar_coluna_por_indice(linha, 111, dataEvolucao, sizeof(dataEvolucao));
        corte = strchr(dataEvolucao, 'T');
        if (corte != NULL) *corte = '\0';
        if (dataEvolucao[0] != '\0')
            snprintf(registro.desfecho, sizeof(registro.desfecho), "%s (%s)", textoEvolucao, dataEvolucao);
        else
            snprintf(registro.desfecho, sizeof(registro.desfecho), "%s", textoEvolucao);

        if (total == capacidade) {
            capacidade *= 2;
            RegistroPaciente* novo = realloc(dados, (size_t)capacidade * sizeof(*dados));
            if (novo == NULL) break;
            dados = novo;
        }
        dados[total++] = registro;
    }
    fclose(arquivo);
    *registros = dados;
    return total;
}
