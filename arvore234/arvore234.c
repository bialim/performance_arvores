#include "arvore234.h"
#include <stdint.h>

Metricas234 g_metricas234 = {0,0,0,0};
void resetMetricas234(void) { g_metricas234 = (Metricas234){0,0,0,0}; }

struct IndiceIds234 {
    long long* ids;
    unsigned char* estados;
    size_t capacidade;
    size_t quantidade;
    size_t ocupados;
};

static size_t hashId234(long long id) {
    uint64_t valor = (uint64_t)id + UINT64_C(0x9e3779b97f4a7c15);
    valor = (valor ^ (valor >> 30)) * UINT64_C(0xbf58476d1ce4e5b9);
    valor = (valor ^ (valor >> 27)) * UINT64_C(0x94d049bb133111eb);
    valor ^= valor >> 31;
    return (size_t)valor;
}

static IndiceIds234* novoIndiceIds234(size_t capacidade) {
    IndiceIds234* indice = malloc(sizeof(*indice));
    if (indice == NULL) { perror("malloc"); exit(EXIT_FAILURE); }
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
    indice->quantidade = 0;
    indice->ocupados = 0;
    return indice;
}

static bool contemId234(const IndiceIds234* indice, long long id) {
    size_t posicao = hashId234(id) & (indice->capacidade - 1);
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[posicao] == 0) return false;
        if (indice->estados[posicao] == 1 && indice->ids[posicao] == id) return true;
        posicao = (posicao + 1) & (indice->capacidade - 1);
    }
    return false;
}

static void redimensionarIndiceIds234(IndiceIds234* indice) {
    IndiceIds234 novo = {0};
    novo.capacidade = indice->capacidade * 2;
    novo.ids = calloc(novo.capacidade, sizeof(*novo.ids));
    novo.estados = calloc(novo.capacidade, sizeof(*novo.estados));
    if (novo.ids == NULL || novo.estados == NULL) {
        perror("calloc");
        free(novo.ids);
        free(novo.estados);
        exit(EXIT_FAILURE);
    }
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[i] != 1) continue;
        size_t posicao = hashId234(indice->ids[i]) & (novo.capacidade - 1);
        while (novo.estados[posicao] == 1)
            posicao = (posicao + 1) & (novo.capacidade - 1);
        novo.ids[posicao] = indice->ids[i];
        novo.estados[posicao] = 1;
        novo.quantidade++;
        novo.ocupados++;
    }
    free(indice->ids);
    free(indice->estados);
    indice->ids = novo.ids;
    indice->estados = novo.estados;
    indice->capacidade = novo.capacidade;
    indice->quantidade = novo.quantidade;
    indice->ocupados = novo.ocupados;
}

static bool inserirId234(IndiceIds234* indice, long long id) {
    if (contemId234(indice, id)) return false;
    if ((indice->ocupados + 1) * 10 >= indice->capacidade * 7)
        redimensionarIndiceIds234(indice);

    size_t posicao = hashId234(id) & (indice->capacidade - 1);
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
    indice->quantidade++;
    return true;
}

static void removerId234(IndiceIds234* indice, long long id) {
    size_t posicao = hashId234(id) & (indice->capacidade - 1);
    for (size_t i = 0; i < indice->capacidade; i++) {
        if (indice->estados[posicao] == 0) return;
        if (indice->estados[posicao] == 1 && indice->ids[posicao] == id) {
            indice->estados[posicao] = 2;
            indice->quantidade--;
            return;
        }
        posicao = (posicao + 1) & (indice->capacidade - 1);
    }
}

static void liberarIndiceIds234(IndiceIds234* indice) {
    if (indice == NULL) return;
    free(indice->ids);
    free(indice->estados);
    free(indice);
}

static void indexarNos234(No234* no, IndiceIds234* indice) {
    if (no == NULL) return;
    for (int i = 0; i < no->numChaves; i++) inserirId234(indice, no->chaves[i].chave.id);
    if (!no->folha)
        for (int i = 0; i < no->numFilhos; i++) indexarNos234(no->filhos[i], indice);
}

static void garantirIndiceIds234(No234* raiz) {
    if (raiz == NULL || raiz->indiceIds != NULL) return;
    IndiceIds234* indice = novoIndiceIds234(16);
    indexarNos234(raiz, indice);
    raiz->indiceIds = indice;
}

/*==============================================================
 *  COMPARAÇÃO LEXICOGRÁFICA
 *==============================================================*/
int compararChaves(const Chave234* a, const Chave234* b) {
    int c = strcmp(a->cidade_estado, b->cidade_estado);
    if (c != 0) return c;
    c = strcmp(a->data_registro, b->data_registro);
    if (c != 0) return c;
    if (a->id < b->id) return -1;
    if (a->id > b->id) return  1;
    return 0;
}

/*==============================================================
 *  CRIAÇÃO E DESTRUIÇÃO
 *==============================================================*/
No234* novoNo234(bool folha) {
    No234* n = (No234*)malloc(sizeof(No234));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->numChaves = 0;
    n->numFilhos = 0;
    n->folha     = folha;
    n->indiceIds = NULL;
    for (int i = 0; i < MAX_FILHOS; i++) n->filhos[i] = NULL;
    return n;
}

void liberarArvore234(No234* r) {
    if (r == NULL) return;
    liberarIndiceIds234(r->indiceIds);
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            liberarArvore234(r->filhos[i]);
    free(r);
}

/*==============================================================
 *  BUSCA
 *==============================================================*/
No234* buscar234(No234* r, const Chave234* chave) {
    while (r != NULL) {
        int i = 0;
        while (i < r->numChaves) {
            g_metricas234.comparacoes++;
            int c = compararChaves(&r->chaves[i].chave, chave);
            if (c == 0) return r;
            if (c > 0) break;
            i++;
        }
        if (r->folha) return NULL;
        r = r->filhos[i];
    }
    return NULL;
}

/*==============================================================
 *  INSERÇÃO
 *==============================================================*/
static void dividirFilho234(No234* pai, int idx) {
    No234* filho = pai->filhos[idx];
    No234* novo  = novoNo234(filho->folha);
    RegistroPaciente meio = filho->chaves[1];

    novo->chaves[0] = filho->chaves[2];
    novo->numChaves = 1;

    if (!filho->folha) {
        novo->filhos[0] = filho->filhos[2];
        novo->filhos[1] = filho->filhos[3];
        novo->numFilhos = 2;
        filho->filhos[2] = NULL;
        filho->filhos[3] = NULL;
        filho->numFilhos = 2;
    }
    filho->numChaves = 1;

    for (int i = pai->numChaves; i > idx; i--)
        pai->chaves[i] = pai->chaves[i - 1];
    for (int i = pai->numFilhos; i > idx + 1; i--)
        pai->filhos[i] = pai->filhos[i - 1];

    pai->chaves[idx]     = meio;
    pai->filhos[idx + 1] = novo;
    pai->numChaves++;
    pai->numFilhos++;
    g_metricas234.splits++;
}

static int inserirRec234(No234* no, RegistroPaciente dado) {
    if (no->folha) {
        int i = no->numChaves - 1;
        while (i >= 0) {
            g_metricas234.comparacoes++;
            int c = compararChaves(&no->chaves[i].chave, &dado.chave);
            if (c == 0) return 0;
            if (c < 0) break;
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = dado;
        no->numChaves++;
        return 1;
    }

    int i = no->numChaves - 1;
    while (i >= 0) {
        g_metricas234.comparacoes++;
        int c = compararChaves(&no->chaves[i].chave, &dado.chave);
        if (c == 0) return 0;
        if (c < 0) break;
        i--;
    }
    i++;

    if (no->filhos[i]->numChaves == MAX_CHAVES) {
        dividirFilho234(no, i);
        g_metricas234.comparacoes++;
        if (compararChaves(&dado.chave, &no->chaves[i].chave) > 0) i++;
    }
    return inserirRec234(no->filhos[i], dado);
}

bool inserirNo234(No234** raizRef, RegistroPaciente dado) {
    if (*raizRef == NULL) {
        *raizRef = novoNo234(true);
        (*raizRef)->indiceIds = novoIndiceIds234(16);
        inserirId234((*raizRef)->indiceIds, dado.chave.id);
        (*raizRef)->chaves[0] = dado;
        (*raizRef)->numChaves = 1;
        g_metricas234.insercoes_ok++;
        return true;
    }
    garantirIndiceIds234(*raizRef);
    if (contemId234((*raizRef)->indiceIds, dado.chave.id)) {
        g_metricas234.insercoes_dup++;
        return false;
    }
    if ((*raizRef)->numChaves == MAX_CHAVES) {
        No234* novaRaiz = novoNo234(false);
        novaRaiz->indiceIds = (*raizRef)->indiceIds;
        (*raizRef)->indiceIds = NULL;
        novaRaiz->filhos[0] = *raizRef;
        novaRaiz->numFilhos = 1;
        dividirFilho234(novaRaiz, 0);
        *raizRef = novaRaiz;
    }
    int ok = inserirRec234(*raizRef, dado);
    if (ok) {
        inserirId234((*raizRef)->indiceIds, dado.chave.id);
        g_metricas234.insercoes_ok++;
    }
    else    g_metricas234.insercoes_dup++;
    return ok != 0;
}

/*==============================================================
 *  REMOÇÃO
 *==============================================================*/
static int acharChaveId234(No234* no, const Chave234* chave) {
    for (int i = 0; i < no->numChaves; i++) {
        g_metricas234.comparacoes++;
        if (compararChaves(&no->chaves[i].chave, chave) == 0) return i;
    }
    return -1;
}

static RegistroPaciente antecessor234(No234* no, int idx) {
    No234* a = no->filhos[idx];
    while (!a->folha) a = a->filhos[a->numChaves];
    return a->chaves[a->numChaves - 1];
}
static RegistroPaciente sucessor234(No234* no, int idx) {
    No234* a = no->filhos[idx + 1];
    while (!a->folha) a = a->filhos[0];
    return a->chaves[0];
}

static void removerDeFolha234(No234* no, int idx) {
    for (int i = idx + 1; i < no->numChaves; i++)
        no->chaves[i - 1] = no->chaves[i];
    no->numChaves--;
}

static void emprestarDoAnterior234(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx - 1];
    for (int i = filho->numChaves - 1; i >= 0; i--)
        filho->chaves[i + 1] = filho->chaves[i];
    if (!filho->folha)
        for (int i = filho->numFilhos - 1; i >= 0; i--)
            filho->filhos[i + 1] = filho->filhos[i];
    filho->chaves[0] = no->chaves[idx - 1];
    filho->numChaves++;
    no->chaves[idx - 1] = irmao->chaves[irmao->numChaves - 1];
    if (!filho->folha) {
        filho->filhos[0] = irmao->filhos[irmao->numFilhos - 1];
        filho->numFilhos++;
        irmao->numFilhos--;
    }
    irmao->numChaves--;
}

static void emprestarDoProximo234(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx + 1];
    filho->chaves[filho->numChaves++] = no->chaves[idx];
    no->chaves[idx] = irmao->chaves[0];
    if (!filho->folha) {
        filho->filhos[filho->numFilhos++] = irmao->filhos[0];
    }
    for (int i = 1; i < irmao->numChaves; i++)
        irmao->chaves[i - 1] = irmao->chaves[i];
    if (!irmao->folha) {
        for (int i = 1; i < irmao->numFilhos; i++)
            irmao->filhos[i - 1] = irmao->filhos[i];
        irmao->numFilhos--;
    }
    irmao->numChaves--;
}

static void fundirNos234(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx + 1];
    filho->chaves[filho->numChaves++] = no->chaves[idx];
    for (int i = 0; i < irmao->numChaves; i++)
        filho->chaves[filho->numChaves++] = irmao->chaves[i];
    if (!filho->folha)
        for (int i = 0; i < irmao->numFilhos; i++)
            filho->filhos[filho->numFilhos++] = irmao->filhos[i];
    for (int i = idx + 1; i < no->numChaves; i++)
        no->chaves[i - 1] = no->chaves[i];
    for (int i = idx + 2; i < no->numFilhos; i++)
        no->filhos[i - 1] = no->filhos[i];
    no->numChaves--;
    no->numFilhos--;
    free(irmao);
}

static void preencherNo234(No234* no, int idx) {
    if (idx > 0 && no->filhos[idx - 1]->numChaves >= 2)
        emprestarDoAnterior234(no, idx);
    else if (idx < no->numChaves && no->filhos[idx + 1]->numChaves >= 2)
        emprestarDoProximo234(no, idx);
    else if (idx < no->numChaves)
        fundirNos234(no, idx);
    else
        fundirNos234(no, idx - 1);
}

static void removerRec234(No234* no, const Chave234* chave);

static void removerDeNaoFolha234(No234* no, int idx) {
    Chave234 ch = no->chaves[idx].chave;
    if (no->filhos[idx]->numChaves >= 2) {
        RegistroPaciente ant = antecessor234(no, idx);
        no->chaves[idx] = ant;
        removerRec234(no->filhos[idx], &ant.chave);
    } else if (no->filhos[idx + 1]->numChaves >= 2) {
        RegistroPaciente suc = sucessor234(no, idx);
        no->chaves[idx] = suc;
        removerRec234(no->filhos[idx + 1], &suc.chave);
    } else {
        fundirNos234(no, idx);
        removerRec234(no->filhos[idx], &ch);
    }
}

static void removerRec234(No234* no, const Chave234* chave) {
    int idx = acharChaveId234(no, chave);
    if (idx != -1) {
        if (no->folha) removerDeFolha234(no, idx);
        else           removerDeNaoFolha234(no, idx);
        return;
    }
    if (no->folha) return;
    int i = no->numChaves - 1;
    while (i >= 0) {
        g_metricas234.comparacoes++;
        if (compararChaves(&no->chaves[i].chave, chave) < 0) break;
        i--;
    }
    i++;
    if (no->filhos[i]->numChaves == 1) {
        preencherNo234(no, i);
        if (i > no->numChaves) i--;
        else if (i < no->numChaves &&
                 compararChaves(&no->chaves[i].chave, chave) < 0) i++;
    }
    removerRec234(no->filhos[i], chave);
}

bool removerNo234(No234** raizRef, const Chave234* chave) {
    if (*raizRef == NULL) return false;
    garantirIndiceIds234(*raizRef);
    if (buscar234(*raizRef, chave) == NULL) return false;
    removerRec234(*raizRef, chave);
    removerId234((*raizRef)->indiceIds, chave->id);
    if ((*raizRef)->numChaves == 0) {
        No234* antiga = *raizRef;
        No234* novaRaiz = antiga->folha ? NULL : antiga->filhos[0];
        if (novaRaiz != NULL) {
            novaRaiz->indiceIds = antiga->indiceIds;
            antiga->indiceIds = NULL;
        }
        *raizRef = novaRaiz;
        liberarIndiceIds234(antiga->indiceIds);
        free(antiga);
    }
    return true;
}

/*==============================================================
 *  PERCURSOS
 *==============================================================*/
void preOrdem234(No234* r) {
    if (r == NULL) return;
    for (int i = 0; i < r->numChaves; i++) {
        printf("[%s|%s|%lld] ", r->chaves[i].chave.cidade_estado,
               r->chaves[i].chave.data_registro, r->chaves[i].chave.id);
        if (!r->folha) preOrdem234(r->filhos[i]);
    }
    if (!r->folha) preOrdem234(r->filhos[r->numChaves]);
}

void emOrdem234(No234* r) {
    if (r == NULL) return;
    for (int i = 0; i < r->numChaves; i++) {
        if (!r->folha) emOrdem234(r->filhos[i]);
        printf("[%s|%s|%lld] ", r->chaves[i].chave.cidade_estado,
               r->chaves[i].chave.data_registro, r->chaves[i].chave.id);
    }
    if (!r->folha) emOrdem234(r->filhos[r->numChaves]);
}

/*==============================================================
 *  ESTATÍSTICAS
 *==============================================================*/
int altura234(No234* r) {
    if (r == NULL) return 0;
    if (r->folha)  return 1;
    return 1 + altura234(r->filhos[0]);
}

int contarNos234(No234* r) {
    if (r == NULL) return 0;
    int t = 1;
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            t += contarNos234(r->filhos[i]);
    return t;
}

int contarChaves234(No234* r) {
    if (r == NULL) return 0;
    int t = r->numChaves;
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            t += contarChaves234(r->filhos[i]);
    return t;
}

size_t estimarMemoria234(No234* r) {
    if (r == NULL) return 0;
    size_t t = sizeof(No234);
    if (r->indiceIds != NULL)
        t += sizeof(*r->indiceIds) + r->indiceIds->capacidade *
             (sizeof(*r->indiceIds->ids) + sizeof(*r->indiceIds->estados));
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            t += estimarMemoria234(r->filhos[i]);
    return t;
}

/*==============================================================
 *  VALIDAÇÃO AUTOMATIZADA
 *==============================================================*/
static int validarRec234(No234* no, int nivel, int nivelFolha,
                         ResultadoValidacao234* res) {
    if (no == NULL) return nivelFolha;

    if (no->numChaves < 1 || no->numChaves > MAX_CHAVES) {
        res->erros++;
        snprintf(res->mensagem, sizeof(res->mensagem),
                 "No com %d chaves (esperado 1..3) nivel %d",
                 no->numChaves, nivel);
        return nivelFolha;
    }
    if (no->folha && no->numFilhos != 0) {
        res->erros++;
        snprintf(res->mensagem, sizeof(res->mensagem),
                 "Folha com %d filhos", no->numFilhos);
    }
    if (!no->folha && no->numFilhos != no->numChaves + 1) {
        res->erros++;
        snprintf(res->mensagem, sizeof(res->mensagem),
                 "No interno com %d chaves e %d filhos (esperado %d)",
                 no->numChaves, no->numFilhos, no->numChaves + 1);
    }
    for (int i = 1; i < no->numChaves; i++) {
        if (compararChaves(&no->chaves[i-1].chave, &no->chaves[i].chave) >= 0) {
            res->erros++;
            snprintf(res->mensagem, sizeof(res->mensagem),
                     "Chaves fora de ordem no nivel %d", nivel);
        }
    }
    if (!no->folha) {
        for (int i = 0; i < no->numFilhos; i++) {
            if (no->filhos[i] == NULL) {
                res->erros++;
                snprintf(res->mensagem, sizeof(res->mensagem),
                         "Filho %d NULL em no nao-folha", i);
                continue;
            }
            nivelFolha = validarRec234(no->filhos[i], nivel + 1,
                                       nivelFolha, res);
        }
        for (int i = 0; i < no->numChaves; i++) {
            No234* esq = no->filhos[i];
            No234* dir = no->filhos[i + 1];
            if (esq->numChaves > 0) {
                int ult = esq->numChaves - 1;
                if (compararChaves(&esq->chaves[ult].chave,
                                   &no->chaves[i].chave) >= 0) {
                    res->erros++;
                    snprintf(res->mensagem, sizeof(res->mensagem),
                             "Intervalo violado a esquerda da chave %d", i);
                }
            }
            if (dir->numChaves > 0) {
                if (compararChaves(&dir->chaves[0].chave,
                                   &no->chaves[i].chave) <= 0) {
                    res->erros++;
                    snprintf(res->mensagem, sizeof(res->mensagem),
                             "Intervalo violado a direita da chave %d", i);
                }
            }
        }
    } else {
        if (nivelFolha == -1) return nivel;
        if (nivel != nivelFolha) {
            res->erros++;
            snprintf(res->mensagem, sizeof(res->mensagem),
                     "Folha no nivel %d (esperado %d)", nivel, nivelFolha);
        }
    }
    return nivelFolha;
}

ResultadoValidacao234 validar234(No234* r) {
    ResultadoValidacao234 res = { true, 0, 0, "" };
    if (r == NULL) return res;
    if (r->numChaves < 1 || r->numChaves > MAX_CHAVES) {
        res.ok = false; res.erros++;
        snprintf(res.mensagem, sizeof(res.mensagem),
                 "Raiz com %d chaves", r->numChaves);
        return res;
    }
    res.alturaFolhas = validarRec234(r, 0, -1, &res);
    res.ok = (res.erros == 0);
    return res;
}

/*==============================================================
 *  CSV
 *==============================================================*/
void pegar_coluna_por_indice(const char* linha, int coluna, char* resultado,
                             size_t capacidade) {
    int col_atual = 0, i = 0, j = 0;
    if (capacidade == 0) return;
    resultado[0] = '\0';
    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        if (col_atual == coluna) {
            while (linha[i] != ';' && linha[i] != '\0' &&
                   linha[i] != '\n' && linha[i] != '\r') {
                if (linha[i] != '"' && (size_t)j + 1 < capacidade)
                    resultado[j++] = linha[i];
                i++;
            }
            resultado[j] = '\0';
            return;
        }
        if (linha[i] == ';') col_atual++;
        i++;
    }
}

static bool registroValido(const RegistroPaciente* r) {
    return r->chave.cidade_estado[0] != '\0' &&
           r->chave.data_registro[0] != '\0' &&
           r->chave.id > 0;
}

long long lerCSV234(const char* nomeArquivo, RegistroPaciente** registros,
                    long long limite) {
    *registros = NULL;
    FILE* f = fopen(nomeArquivo, "r");
    if (!f) {
        char alt[512];
        int n = snprintf(alt, sizeof(alt), "../%s", nomeArquivo);
        if (n > 0 && (size_t)n < sizeof(alt)) f = fopen(alt, "r");
    }
    if (!f) { printf("Erro: nao foi possivel abrir '%s'.\n", nomeArquivo); return 0; }

    static char linha[40000];
    if (!fgets(linha, sizeof(linha), f)) { fclose(f); return 0; }

    long long cap = 1024, n = 0;
    RegistroPaciente* arr = malloc(cap * sizeof(RegistroPaciente));
    if (!arr) { fclose(f); return 0; }

    static char buf[256], estado[10], cidade[90];
    while (fgets(linha, sizeof(linha), f)) {
        if (limite > 0 && n >= limite) break;

        RegistroPaciente reg;
        memset(&reg, 0, sizeof(reg));

        pegar_coluna_por_indice(linha, 0, buf, sizeof(buf));
        reg.chave.id = atoll(buf);
        pegar_coluna_por_indice(linha, 5, estado, sizeof(estado));
        pegar_coluna_por_indice(linha, 8, cidade, sizeof(cidade));
        snprintf(reg.chave.cidade_estado, TAM_CIDADE, "%s-%s", cidade, estado);
        pegar_coluna_por_indice(linha, 1, reg.chave.data_registro,
                    sizeof(reg.chave.data_registro));
        char* T = strchr(reg.chave.data_registro, 'T');
        if (T) *T = '\0';
        pegar_coluna_por_indice(linha, 10, reg.sexo, sizeof(reg.sexo));
        pegar_coluna_por_indice(linha, 11, reg.data_nascimento,
                    sizeof(reg.data_nascimento));
        T = strchr(reg.data_nascimento, 'T');
        if (T) *T = '\0';

        char evo[10], evo_txt[30], data_evo[25];
        pegar_coluna_por_indice(linha, 110, evo, sizeof(evo));
        if      (!strcmp(evo,"1") || !strcmp(evo,"1.0")) strcpy(evo_txt,"Cura");
        else if (!strcmp(evo,"2") || !strcmp(evo,"2.0")) strcpy(evo_txt,"Obito");
        else if (!strcmp(evo,"3") || !strcmp(evo,"3.0")) strcpy(evo_txt,"Outras causas");
        else strcpy(evo_txt,"Desconhecido");
        pegar_coluna_por_indice(linha, 111, data_evo, sizeof(data_evo));
        T = strchr(data_evo, 'T'); if (T) *T = '\0';
        if (strlen(data_evo) > 0)
            snprintf(reg.desfecho, sizeof(reg.desfecho), "%s (%s)", evo_txt, data_evo);
        else
            snprintf(reg.desfecho, sizeof(reg.desfecho), "%s", evo_txt);

        if (!registroValido(&reg)) continue;
        if (n == cap) {
            cap *= 2;
            RegistroPaciente* novo = realloc(arr, cap * sizeof(RegistroPaciente));
            if (!novo) { free(arr); fclose(f); return n; }
            arr = novo;
        }
        arr[n++] = reg;
    }
    fclose(f);
    *registros = arr;
    return n;
}

/*==============================================================
 *  IMPRESSÃO EM TABELA
 *==============================================================*/
void imprimirCabecalhoTabela234(void) {
    printf("+---------------------+------------+----------------------+----------------+------+----------------------+\n");
    printf("| Cidade/UF           | Registro   | ID                   | Nascimento     | Sexo | Desfecho             |\n");
    printf("+---------------------+------------+----------------------+----------------+------+----------------------+\n");
}

static void imprimirRec234(No234* r, long long* cont, long long lim) {
    if (r == NULL || *cont >= lim) return;
    for (int i = 0; i < r->numChaves; i++) {
        if (!r->folha) imprimirRec234(r->filhos[i], cont, lim);
        if (*cont >= lim) return;
        if (*cont % 40 == 0) imprimirCabecalhoTabela234();
         printf("| %-19.19s | %-10.10s | %-20lld | %-14.14s | %-4.4s | %-20.20s |\n",
               r->chaves[i].chave.cidade_estado,
               r->chaves[i].chave.data_registro,
             r->chaves[i].chave.id,
               r->chaves[i].data_nascimento,
               r->chaves[i].sexo,
               r->chaves[i].desfecho);
        (*cont)++;
    }
    if (!r->folha) imprimirRec234(r->filhos[r->numChaves], cont, lim);
}

void imprimirEmOrdemTabela234(No234* raiz, long long* contador, long long limite) {
    imprimirRec234(raiz, contador, limite);
}