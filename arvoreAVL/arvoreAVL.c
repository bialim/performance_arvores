#include <stdio.h>
#include <stdlib.h>
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

// Estrutura que define cada no da Arvore AVL, contendo os dados, altura e ponteiros para os filhos.
typedef struct NoAVL {
    RegistroPaciente dado;
    int altura;
    struct NoAVL *esquerda;
    struct NoAVL *direita;
} NoAVL;

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
    no->dado = dado;
    no->esquerda = NULL;
    no->direita = NULL;
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
    return y;
}

// Calcula e retorna o fator de balanceamento de um no (diferenca entre a altura da esquerda e da direita).
int obterBalanceamento(NoAVL *N) {
    if (N == NULL) return 0;
    return altura(N->esquerda) - altura(N->direita);
}

// Insere um novo registo de paciente na arvore AVL de forma ordenada por ID e aplica as rotacoes se necessario.
NoAVL* inserirAVL(NoAVL* no, RegistroPaciente dado) {
    if (no == NULL) return novoNo(dado);
    if (dado.id < no->dado.id) no->esquerda = inserirAVL(no->esquerda, dado);
    else if (dado.id > no->dado.id) no->direita = inserirAVL(no->direita, dado);
    else return no;

    no->altura = 1 + max(altura(no->esquerda), altura(no->direita));
    int balanceamento = obterBalanceamento(no);

    if (balanceamento > 1 && dado.id < no->esquerda->dado.id) return rotacaoDireita(no);
    if (balanceamento < -1 && dado.id > no->direita->dado.id) return rotacaoEsquerda(no);
    if (balanceamento > 1 && dado.id > no->esquerda->dado.id) {
        no->esquerda = rotacaoEsquerda(no->esquerda);
        return rotacaoDireita(no);
    }
    if (balanceamento < -1 && dado.id < no->direita->dado.id) {
        no->direita = rotacaoDireita(no->direita);
        return rotacaoEsquerda(no);
    }
    return no;
}

// Extrai o texto de uma coluna especifica de uma linha do CSV com base no separador ponto e virgula (;), ignorando aspas.
void pegar_coluna_por_indice(const char *linha, int coluna_desejada, char *resultado) {
    int col_atual = 0;
    int i = 0, j = 0;
    resultado[0] = '\0';

    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        if (col_atual == coluna_desejada) {
            while (linha[i] != ';' && linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
                if (linha[i] != '"') {
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
void imprimirCabecalhoTabela() {
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
    printf("| ID Paciente    | Registro     | Nascimento   | Cidade e Estado           | Sexo | Desfecho             |\n");
    printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
}

// Percorre a arvore em ordem (In-Order) e imprime os dados dos pacientes formatados em forma de tabela na consola.
void imprimirEmOrdemTabela(NoAVL *raiz, int *contador_linhas) {
    if (raiz != NULL) {
        imprimirEmOrdemTabela(raiz->esquerda, contador_linhas);

        if (*contador_linhas % 40 == 0) {
            imprimirCabecalhoTabela();
        }

        printf("| %-14lld | %-12.12s | %-12.12s | %-25.25s | %-4.4s | %-20.20s |\n",
               raiz->dado.id,
               raiz->dado.data_registro,
               raiz->dado.data_nascimento,
               raiz->dado.cidade_estado,
               raiz->dado.sexo,
               raiz->dado.desfecho);

        (*contador_linhas)++;

        imprimirEmOrdemTabela(raiz->direita, contador_linhas);
    }
}

// Abre o ficheiro CSV, le as linhas, processa os dados, aplica filtros e insere os registos na arvore AVL.
NoAVL* carregarPlanilhaCSV(const char* nomeArquivo, NoAVL* raiz, int *total_carregado) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) {
        printf("Erro: Nao foi possivel abrir o ficheiro '%s'.\n", nomeArquivo);
        return raiz;
    }

    char linha[30000];
    char buffer_temp[255];
    char estado[10], cidade[90];

    fgets(linha, sizeof(linha), arquivo);

    *total_carregado = 0;

    while (fgets(linha, sizeof(linha), arquivo) && *total_carregado < 40) {
        RegistroPaciente reg;
        memset(&reg, 0, sizeof(RegistroPaciente));

        pegar_coluna_por_indice(linha, 0, buffer_temp);
        reg.id = atoll(buffer_temp);
        if (reg.id == 0) continue;

        pegar_coluna_por_indice(linha, 1, reg.data_registro);
        char *corte_registo = strchr(reg.data_registro, 'T');
        if (corte_registo != NULL) *corte_registo = '\0';

        pegar_coluna_por_indice(linha, 5, estado);
        pegar_coluna_por_indice(linha, 8, cidade);
        sprintf(reg.cidade_estado, "%s-%s", cidade, estado);

        pegar_coluna_por_indice(linha, 10, reg.sexo);

        pegar_coluna_por_indice(linha, 11, reg.data_nascimento);
        char *corte_nasc = strchr(reg.data_nascimento, 'T');
        if (corte_nasc != NULL) *corte_nasc = '\0';

        char evolucao_bruto[10];
        char evolucao_texto[25];
        char data_evo[25];

        pegar_coluna_por_indice(linha, 110, evolucao_bruto);
        if(strcmp(evolucao_bruto, "1.0") == 0 || strcmp(evolucao_bruto, "1") == 0) strcpy(evolucao_texto, "Cura");
        else if(strcmp(evolucao_bruto, "2.0") == 0 || strcmp(evolucao_bruto, "2") == 0) strcpy(evolucao_texto, "Obito");
        else if(strcmp(evolucao_bruto, "3.0") == 0 || strcmp(evolucao_bruto, "3") == 0) strcpy(evolucao_texto, "Outras causas");
        else strcpy(evolucao_texto, "Desconhecido");

        pegar_coluna_por_indice(linha, 111, data_evo);
        char *corte_evo = strchr(data_evo, 'T');
        if (corte_evo != NULL) *corte_evo = '\0';

        if (strlen(data_evo) > 0) sprintf(reg.desfecho, "%s (%s)", evolucao_texto, data_evo);
        else sprintf(reg.desfecho, "%s", evolucao_texto);

        raiz = inserirAVL(raiz, reg);
        (*total_carregado)++;
    }

    fclose(arquivo);
    return raiz;
}

// Funcao principal que controla o fluxo do programa, exibe o titulo, carrega o CSV e imprime a tabela.
int main() {
    NoAVL* raiz = NULL;
    int total_pacientes = 0;
    int contador_linhas = 0;

    // Titulo principal no topo do programa
    printf("==========================================================================================================\n");
    printf("                         Dados SIVEP-Gripe: Casos de SRAG no Brasil (2019-2026)                           \n");
    printf("==========================================================================================================\n\n");

    printf("A ler o ficheiro CSV (limitado a 40 pacientes)...\n\n");

    raiz = carregarPlanilhaCSV("INFLUD19-23-03-2026.csv", raiz, &total_pacientes);

    if (raiz != NULL) {
        imprimirEmOrdemTabela(raiz, &contador_linhas);

        printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
        printf("Total de pacientes carregados e balanceados na Arvore: %d\n\n", total_pacientes);

    } else {
        printf("A arvore esta vazia ou o ficheiro nao foi encontrado.\n");
    }

    return 0;
}
