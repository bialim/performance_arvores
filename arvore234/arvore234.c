#include "arvore234.h"

/*==============================================================
 *  CRIACAO E DESTRUICAO
 *==============================================================*/

// Aloca dinamicamente na memoria um novo no da arvore, indicando se e folha ou nao.
No234* novoNo234(bool folha) {
    No234* n = (No234*)malloc(sizeof(No234));
    if (!n) { perror("malloc"); exit(EXIT_FAILURE); }
    n->numChaves = 0;
    n->numFilhos = 0;
    n->folha     = folha;
    for (int i = 0; i < MAX_FILHOS; i++) n->filhos[i] = NULL;
    return n;
}

// Liberta recursivamente toda a memoria ocupada por uma subarvore a partir da raiz fornecida.
void liberarArvore234(No234* r) {
    if (r == NULL) return;
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            liberarArvore234(r->filhos[i]);
    free(r);
}

/*==============================================================
 *  BUSCA
 *==============================================================*/

// Pesquisa um registo pela chave (id) percorrendo a arvore da raiz para as folhas.
// A cada no, compara a chave procurada com as chaves armazenadas para decidir
// qual subarvore (esquerda, do meio ou direita) deve ser seguida.
No234* buscar234(No234* r, long long id) {
    while (r != NULL) {
        int i = 0;
        while (i < r->numChaves && id > r->chaves[i].id) i++;
        if (i < r->numChaves && id == r->chaves[i].id) return r;
        if (r->folha) return NULL;
        r = r->filhos[i];
    }
    return NULL;
}

/*==============================================================
 *  INSERCAO
 *==============================================================*/

// Divide um no-filho que esta cheio (4-no), promovendo a chave do meio para o pai
// e criando um novo no irmao a direita que recebe a maior chave do filho original.
static void dividirFilho234(No234* pai, int idx) {
    No234* filho = pai->filhos[idx];
    No234* novo  = novoNo234(filho->folha);
    RegistroPaciente meio = filho->chaves[1];

    // Novo no recebe a maior chave do filho
    novo->chaves[0] = filho->chaves[2];
    novo->numChaves = 1;

    // Transfere os dois ultimos filhos (se o no nao for folha)
    if (!filho->folha) {
        novo->filhos[0] = filho->filhos[2];
        novo->filhos[1] = filho->filhos[3];
        novo->numFilhos = 2;
    }

    // Filho vira um 2-no com a menor chave
    filho->numChaves = 1;

    // Abre espaco no pai para receber a chave promovida
    for (int i = pai->numChaves; i > idx; i--)
        pai->chaves[i] = pai->chaves[i - 1];
    for (int i = pai->numFilhos; i > idx + 1; i--)
        pai->filhos[i] = pai->filhos[i - 1];

    // Sobe a chave do meio e conecta o novo no como filho do pai
    pai->chaves[idx]     = meio;
    pai->filhos[idx + 1] = novo;
    pai->numChaves++;
    pai->numFilhos++;
}

// Desce recursivamente ate uma folha inserindo a chave na posicao ordenada.
// Antes de descer, se o filho for um 4-no cheio, ele e dividido preemptivamente.
static void inserirRec(No234* no, RegistroPaciente dado) {
    // Caso base: chegou a uma folha, insere a chave mantendo a ordenacao
    if (no->folha) {
        int i = no->numChaves - 1;
        while (i >= 0 && no->chaves[i].id > dado.id) {
            no->chaves[i + 1] = no->chaves[i];
            i--;
        }
        no->chaves[i + 1] = dado;
        no->numChaves++;
        return;
    }

    // Encontra o filho apropriado para descer
    int i = no->numChaves - 1;
    while (i >= 0 && no->chaves[i].id > dado.id) i--;
    i++;

    // Se o filho for um 4-no cheio, divide antes de descer (split preemptivo)
    if (no->filhos[i]->numChaves == MAX_CHAVES) {
        dividirFilho234(no, i);
        if (dado.id > no->chaves[i].id) i++;
    }

    inserirRec(no->filhos[i], dado);
}

// Insere um novo registo de paciente na arvore 2-3-4. Se a raiz estiver cheia,
// ela e dividida primeiro, aumentando a altura da arvore em um nivel.
No234* inserirNo234(No234* r, RegistroPaciente dado) {
    if (r == NULL) {
        r = novoNo234(true);
        r->chaves[0] = dado;
        r->numChaves = 1;
        return r;
    }

    // Divisao da raiz: cria nova raiz e aumenta a altura
    if (r->numChaves == MAX_CHAVES) {
        No234* novaRaiz = novoNo234(false);
        novaRaiz->filhos[0] = r;
        novaRaiz->numFilhos = 1;
        dividirFilho234(novaRaiz, 0);
        r = novaRaiz;
    }

    inserirRec(r, dado);
    return r;
}

/*==============================================================
 *  REMOCAO
 *==============================================================*/

// Procura o indice de uma chave pelo id dentro de um no. Retorna -1 se nao encontrar.
static int acharChaveId(No234* no, long long id) {
    for (int i = 0; i < no->numChaves; i++)
        if (no->chaves[i].id == id) return i;
    return -1;
}

// Retorna o registro antecessor (maior chave da subarvore esquerda) de uma chave no indice indicado.
static RegistroPaciente antecessor(No234* no, int idx) {
    No234* a = no->filhos[idx];
    while (!a->folha) a = a->filhos[a->numChaves];
    return a->chaves[a->numChaves - 1];
}

// Retorna o registro sucessor (menor chave da subarvore direita) de uma chave no indice indicado.
static RegistroPaciente sucessor(No234* no, int idx) {
    No234* a = no->filhos[idx + 1];
    while (!a->folha) a = a->filhos[0];
    return a->chaves[0];
}

// Remove diretamente uma chave de um no-folha, deslocando as chaves restantes.
static void removerDeFolha(No234* no, int idx) {
    for (int i = idx + 1; i < no->numChaves; i++)
        no->chaves[i - 1] = no->chaves[i];
    no->numChaves--;
}

// Pega emprestada uma chave do no-irmao anterior e a promove via no-pai (redistribuicao).
static void emprestarDoAnterior(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx - 1];

    // Abre espaco no filho para receber a chave do pai
    for (int i = filho->numChaves - 1; i >= 0; i--)
        filho->chaves[i + 1] = filho->chaves[i];
    if (!filho->folha)
        for (int i = filho->numFilhos - 1; i >= 0; i--)
            filho->filhos[i + 1] = filho->filhos[i];

    // Chave do pai desce para o filho
    filho->chaves[0] = no->chaves[idx - 1];
    filho->numChaves++;

    // Ultima chave do irmao sobe para o pai
    no->chaves[idx - 1] = irmao->chaves[irmao->numChaves - 1];

    // Move o ultimo filho do irmao para o filho
    if (!filho->folha) {
        filho->filhos[0] = irmao->filhos[irmao->numFilhos - 1];
        filho->numFilhos++;
        irmao->numFilhos--;
    }
    irmao->numChaves--;
}

// Pega emprestada uma chave do proximo no-irmao e a promove via no-pai (redistribuicao).
static void emprestarDoProximo(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx + 1];

    // Chave do pai desce para o filho
    filho->chaves[filho->numChaves] = no->chaves[idx];
    filho->numChaves++;

    // Primeira chave do irmao sobe para o pai
    no->chaves[idx] = irmao->chaves[0];

    // Move o primeiro filho do irmao para o filho
    if (!filho->folha) {
        filho->filhos[filho->numFilhos] = irmao->filhos[0];
        filho->numFilhos++;
    }

    // Desloca as chaves e filhos restantes do irmao
    for (int i = 1; i < irmao->numChaves; i++)
        irmao->chaves[i - 1] = irmao->chaves[i];
    if (!irmao->folha) {
        for (int i = 1; i < irmao->numFilhos; i++)
            irmao->filhos[i - 1] = irmao->filhos[i];
        irmao->numFilhos--;
    }
    irmao->numChaves--;
}

// Funde dois nos irmaos adjacentes em um unico no, trazendo a chave separadora do pai.
static void fundirNos(No234* no, int idx) {
    No234* filho = no->filhos[idx];
    No234* irmao = no->filhos[idx + 1];

    // Chave do pai desce para o filho
    filho->chaves[filho->numChaves] = no->chaves[idx];
    filho->numChaves++;

    // Copia todas as chaves do irmao para o filho
    for (int i = 0; i < irmao->numChaves; i++)
        filho->chaves[filho->numChaves++] = irmao->chaves[i];
    // Copia todos os filhos do irmao para o filho
    if (!filho->folha)
        for (int i = 0; i < irmao->numFilhos; i++)
            filho->filhos[filho->numFilhos++] = irmao->filhos[i];

    // Remove a chave separadora e o ponteiro do pai para o irmao fundido
    for (int i = idx + 1; i < no->numChaves; i++)
        no->chaves[i - 1] = no->chaves[i];
    for (int i = idx + 2; i < no->numFilhos; i++)
        no->filhos[i - 1] = no->filhos[i];
    no->numChaves--;
    no->numFilhos--;

    free(irmao);
}

// Garante que o filho no indice indicado tenha pelo menos 2 chaves antes de descer,
// pegando emprestado de um irmao ou fundindo-se com ele.
static void preencherNo(No234* no, int idx) {
    if (idx > 0 && no->filhos[idx - 1]->numChaves >= 2) {
        emprestarDoAnterior(no, idx);
    } else if (idx < no->numChaves && no->filhos[idx + 1]->numChaves >= 2) {
        emprestarDoProximo(no, idx);
    } else if (idx < no->numChaves) {
        fundirNos(no, idx);
    } else {
        fundirNos(no, idx - 1);
    }
}

// Prototipo antecipado da funcao recursiva de remocao (necessario pois ha uso mutuo).
static void removerRec(No234* no, long long id);

// Remove uma chave que esta em um no interno, substituindo-a pelo antecessor ou sucessor
// em ordem, ou fundindo os filhos quando ambos possuem apenas uma chave.
static void removerDeNaoFolha(No234* no, int idx) {
    long long id = no->chaves[idx].id;

    if (no->filhos[idx]->numChaves >= 2) {
        // Caso 1: filho esquerdo tem chaves suficientes -> usa antecessor
        RegistroPaciente ant = antecessor(no, idx);
        no->chaves[idx] = ant;
        removerRec(no->filhos[idx], ant.id);
    } else if (no->filhos[idx + 1]->numChaves >= 2) {
        // Caso 2: filho direito tem chaves suficientes -> usa sucessor
        RegistroPaciente suc = sucessor(no, idx);
        no->chaves[idx] = suc;
        removerRec(no->filhos[idx + 1], suc.id);
    } else {
        // Caso 3: ambos os filhos tem 1 chave -> funde e continua a remocao
        fundirNos(no, idx);
        removerRec(no->filhos[idx], id);
    }
}

// Desce recursivamente pela arvore removendo a chave, garantindo antes de descer
// que o filho tenha chaves suficientes para nao virar um no vazio.
static void removerRec(No234* no, long long id) {
    int idx = acharChaveId(no, id);
    if (idx != -1) {
        if (no->folha) removerDeFolha(no, idx);
        else           removerDeNaoFolha(no, idx);
        return;
    }
    if (no->folha) return;   // chave nao encontrada

    // Localiza o filho onde a chave pode estar
    int i = no->numChaves - 1;
    while (i >= 0 && no->chaves[i].id > id) i--;
    i++;

    // Garante que o filho tenha >= 2 chaves antes de descer
    if (no->filhos[i]->numChaves == 1) {
        preencherNo(no, i);
        if (i > no->numChaves) i--;
        else if (i < no->numChaves && no->chaves[i].id < id) i++;
    }
    removerRec(no->filhos[i], id);
}

// Remove um registro pela chave (id). Se apos a remocao a raiz ficar sem chaves,
// ela e descartada e seu unico filho passa a ser a nova raiz (diminuindo a altura).
No234* removerNo234(No234* r, long long id) {
    if (r == NULL) return NULL;
    if (buscar234(r, id) == NULL) return r;

    removerRec(r, id);

    if (r->numChaves == 0) {
        No234* antiga = r;
        r = r->folha ? NULL : r->filhos[0];
        free(antiga);
    }
    return r;
}

/*==============================================================
 *  PERCURSOS
 *==============================================================*/

// Percorre a arvore em pre-ordem imprimindo apenas os ids na consola.
void preOrdem234(No234* r) {
    if (r == NULL) return;
    for (int i = 0; i < r->numChaves; i++) {
        printf("%lld ", r->chaves[i].id);
        if (!r->folha) preOrdem234(r->filhos[i]);
    }
    if (!r->folha) preOrdem234(r->filhos[r->numChaves]);
}

// Percorre a arvore em ordem (In-Order) imprimindo apenas os ids na consola.
void emOrdem234(No234* r) {
    if (r == NULL) return;
    for (int i = 0; i < r->numChaves; i++) {
        if (!r->folha) emOrdem234(r->filhos[i]);
        printf("%lld ", r->chaves[i].id);
    }
    if (!r->folha) emOrdem234(r->filhos[r->numChaves]);
}

/*==============================================================
 *  ESTATISTICAS
 *==============================================================*/

// Retorna a altura da arvore 2-3-4 (numero de niveis desde a raiz ate uma folha).
// Como todas as folhas estao no mesmo nivel, basta seguir o primeiro filho.
int altura234(No234* r) {
    if (r == NULL) return 0;
    if (r->folha)  return 1;
    return 1 + altura234(r->filhos[0]);
}

// Conta e retorna o numero total de nos existentes na arvore de forma recursiva.
int contarNos234(No234* r) {
    if (r == NULL) return 0;
    int total = 1;
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            total += contarNos234(r->filhos[i]);
    return total;
}

// Conta e retorna o numero total de chaves (registos) armazenados na arvore de forma recursiva.
int contarChaves234(No234* r) {
    if (r == NULL) return 0;
    int total = r->numChaves;
    if (!r->folha)
        for (int i = 0; i < r->numFilhos; i++)
            total += contarChaves234(r->filhos[i]);
    return total;
}

/*==============================================================
 *  LEITURA DO CSV
 *==============================================================*/

// Extrai o texto de uma coluna especifica de uma linha do CSV com base no separador
// ponto e virgula (;), ignorando aspas. Se a coluna nao existir, retorna string vazia.
void pegar_coluna_por_indice(const char *linha, int coluna_desejada, char *resultado) {
    int col_atual = 0;
    int i = 0, j = 0;
    resultado[0] = '\0';

    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        if (col_atual == coluna_desejada) {
            while (linha[i] != ';' && linha[i] != '\0' &&
                   linha[i] != '\n' && linha[i] != '\r') {
                if (linha[i] != '"') resultado[j++] = linha[i];
                i++;
            }
            resultado[j] = '\0';
            return;
        }
        if (linha[i] == ';') col_atual++;
        i++;
    }
}

// Abre o ficheiro CSV, le as linhas, processa os dados e insere os registos na arvore 2-3-4.
// Cada linha gera um RegistroPaciente que e inserido usando seu id como chave de ordenacao.
No234* carregarPlanilhaCSV234(const char* nomeArquivo, No234* raiz,
                              long long* total_carregado) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir '%s'.\n", nomeArquivo);
        printf("Dica: rode o programa a partir da pasta onde o CSV esta,\n");
        printf("      ou informe o caminho completo do arquivo.\n");
        return raiz;
    }

    // Buffers grandes: linhas do SIVEP podem passar de 20 KB
    static char linha[40000];
    static char buffer_temp[256];
    static char estado[10], cidade[90];

    // Pula a linha de cabecalho do CSV
    if (!fgets(linha, sizeof(linha), arquivo)) {
        fclose(arquivo);
        return raiz;
    }

    *total_carregado = 0;

    while (fgets(linha, sizeof(linha), arquivo)) {
        RegistroPaciente reg;
        memset(&reg, 0, sizeof(RegistroPaciente));

        // Coluna 0: ID do paciente (chave de ordenacao)
        pegar_coluna_por_indice(linha, 0, buffer_temp);
        reg.id = atoll(buffer_temp);
        if (reg.id == 0) continue;

        // Coluna 1: data de registro (corta a parte depois do 'T' se houver)
        pegar_coluna_por_indice(linha, 1, reg.data_registro);
        char *corte_registo = strchr(reg.data_registro, 'T');
        if (corte_registo) *corte_registo = '\0';

        // Colunas 5 e 8: estado e cidade (concatenados em "cidade-UF")
        pegar_coluna_por_indice(linha, 5, estado);
        pegar_coluna_por_indice(linha, 8, cidade);
        sprintf(reg.cidade_estado, "%s-%s", cidade, estado);

        // Coluna 10: sexo do paciente
        pegar_coluna_por_indice(linha, 10, reg.sexo);

        // Coluna 11: data de nascimento (corta a parte depois do 'T' se houver)
        pegar_coluna_por_indice(linha, 11, reg.data_nascimento);
        char *corte_nasc = strchr(reg.data_nascimento, 'T');
        if (corte_nasc) *corte_nasc = '\0';

        // Coluna 110: codigo da evolucao clinica (1=Cura, 2=Obito, 3=Outras)
        char evolucao_bruto[10];
        char evolucao_texto[25];
        char data_evo[25];

        pegar_coluna_por_indice(linha, 110, evolucao_bruto);
        if      (!strcmp(evolucao_bruto, "1.0") || !strcmp(evolucao_bruto, "1"))
            strcpy(evolucao_texto, "Cura");
        else if (!strcmp(evolucao_bruto, "2.0") || !strcmp(evolucao_bruto, "2"))
            strcpy(evolucao_texto, "Obito");
        else if (!strcmp(evolucao_bruto, "3.0") || !strcmp(evolucao_bruto, "3"))
            strcpy(evolucao_texto, "Outras causas");
        else
            strcpy(evolucao_texto, "Desconhecido");

        // Coluna 111: data da evolucao (corta a parte depois do 'T' se houver)
        pegar_coluna_por_indice(linha, 111, data_evo);
        char *corte_evo = strchr(data_evo, 'T');
        if (corte_evo) *corte_evo = '\0';

        // Monta o campo desfecho final no formato "Texto (data)" ou so "Texto"
        if (strlen(data_evo) > 0)
            sprintf(reg.desfecho, "%s (%s)", evolucao_texto, data_evo);
        else
            sprintf(reg.desfecho, "%s", evolucao_texto);

        // Insere o registro na arvore 2-3-4
        raiz = inserirNo234(raiz, reg);
        (*total_carregado)++;
    }

    fclose(arquivo);
    return raiz;
}

/*==============================================================
 *  IMPRESSAO EM TABELA
 *==============================================================*/

// Imprime na consola o cabecalho formatado da tabela de dados dos pacientes.
void imprimirCabecalhoTabela234(void) {
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
    printf("| ID Paciente    | Registro     | Nascimento   | Cidade e Estado           | Sexo | Desfecho             |\n");
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
}

// Percorre a arvore em ordem (In-Order) recursivamente imprimindo cada chave e seus dados
// formatados em forma de tabela, respeitando o limite maximo de linhas informado.
static void imprimirEmOrdemTabelaRec(No234* r, long long* contador,
                                     long long limite) {
    if (r == NULL) return;

    for (int i = 0; i < r->numChaves; i++) {
        // Visita a subarvore a esquerda da chave i
        if (!r->folha) imprimirEmOrdemTabelaRec(r->filhos[i], contador, limite);

        if (*contador >= limite) return;

        // Imprime cabecalho da tabela a cada 40 linhas
        if (*contador % 40 == 0) imprimirCabecalhoTabela234();

        printf("| %-14lld | %-12.12s | %-12.12s | %-25.25s | %-4.4s | %-20.20s |\n",
               r->chaves[i].id,
               r->chaves[i].data_registro,
               r->chaves[i].data_nascimento,
               r->chaves[i].cidade_estado,
               r->chaves[i].sexo,
               r->chaves[i].desfecho);

        (*contador)++;
    }

    // Visita a ultima subarvore (a direita da ultima chave do no)
    if (!r->folha)
        imprimirEmOrdemTabelaRec(r->filhos[r->numChaves], contador, limite);
}

// Percorre a arvore em ordem (In-Order) e imprime os dados dos pacientes formatados
// em forma de tabela na consola, limitando a quantidade de linhas exibidas.
void imprimirEmOrdemTabela234(No234* raiz, long long* contador, long long limite) {
    imprimirEmOrdemTabelaRec(raiz, contador, limite);
}