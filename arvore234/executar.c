#include "arvore234.h"
#include <locale.h>

// Imprime na consola o menu principal com as opcoes disponiveis para o usuario.
static void exibirMenu(void) {
    printf("\n");
    printf("================================================================\n");
    printf("        ARVORE 2-3-4  --  SIVEP-Gripe\n");
    printf("================================================================\n");
    printf("   1. Carregar dados\n");
    printf("   2. Buscar paciente por ID\n");
    printf("   3. Remover paciente por ID\n");
    printf("   4. Imprimir em-ordem (tabela, com limite)\n");
    printf("   5. Estatisticas da arvore\n");
    printf("   6. Limpar arvore\n");
    printf("   0. Sair\n");
    printf("================================================================\n");
    printf("Opcao: ");
}

// Le um numero inteiro longo (long long) da entrada padrao, repetindo ate o valor ser valido.
static long long lerLongLong(const char* msg) {
    long long v;
    char buf[64];
    while (1) {
        printf("%s", msg);
        if (fgets(buf, sizeof(buf), stdin) && sscanf(buf, "%lld", &v) == 1)
            return v;
        printf("  Entrada invalida.\n");
    }
}

// Aguarda o usuario pressionar ENTER para continuar apos uma operacao.
static void pausar(void) {
    printf("\nPressione ENTER para continuar...");
    getchar();
}

// Funcao principal que controla o fluxo do programa, exibe o menu e executa as operacoes escolhidas.
int main(void) {
    setlocale(LC_ALL, "Portuguese");

    No234* raiz = NULL;
    long long total_carregado = 0;
    int opcao;

    printf("================================================================\n");
    printf("   ARVORE 2-3-4  --  Leitura do SIVEP-Gripe\n");
    printf("================================================================\n");

    do {
        exibirMenu();
        if (scanf("%d", &opcao) != 1) {
            int c; while ((c = getchar()) != '\n' && c != EOF);
            opcao = -1;
        }
        int c; while ((c = getchar()) != '\n' && c != EOF);

        switch (opcao) {

        // Opcao 1: le o CSV e constroi a arvore 2-3-4 com os registros do SIVEP-Gripe.
        case 1: {
            char caminho[256];
            printf("Nome do arquivo CSV (ex: INFLUD19-23-03-2026.csv): ");
            if (fgets(caminho, sizeof(caminho), stdin))
                caminho[strcspn(caminho, "\r\n")] = '\0';

            printf("\nCarregando '%s'...\n", caminho);
            raiz = carregarPlanilhaCSV234(caminho, raiz, &total_carregado);

            if (total_carregado == 0) {
                printf("Nenhum registro carregado.\n");
            } else {
                printf("\n=== Carga concluida ===\n");
                printf("  Registros inseridos  : %lld\n", total_carregado);
                printf("  Nos totais           : %d\n",   contarNos234(raiz));
                printf("  Chaves totais        : %d\n",   contarChaves234(raiz));
                printf("  Altura da arvore     : %d\n",   altura234(raiz));
            }
            pausar();
            break;
        }

        // Opcao 2: busca um paciente pelo ID e imprime seus dados se encontrado.
        case 2: {
            long long id = lerLongLong("ID a buscar: ");
            No234* no = buscar234(raiz, id);

            if (no != NULL) {
                int idx = -1;
                for (int i = 0; i < no->numChaves; i++)
                    if (no->chaves[i].id == id) { idx = i; break; }
                printf("\n  ENCONTRADO:\n");
                printf("    ID          : %lld\n", no->chaves[idx].id);
                printf("    Registro    : %s\n",   no->chaves[idx].data_registro);
                printf("    Nascimento  : %s\n",   no->chaves[idx].data_nascimento);
                printf("    Cidade/UF   : %s\n",   no->chaves[idx].cidade_estado);
                printf("    Sexo        : %s\n",   no->chaves[idx].sexo);
                printf("    Desfecho    : %s\n",   no->chaves[idx].desfecho);
            } else {
                printf("\n  NAO encontrado.\n");
            }
            pausar();
            break;
        }

        // Opcao 3: remove um paciente pelo ID, reequilibrando a arvore.
        case 3: {
            long long id = lerLongLong("ID a remover: ");
            raiz = removerNo234(raiz, id);
            printf("  Operacao concluida.\n");
            pausar();
            break;
        }

        // Opcao 4: imprime os registros em ordem crescente de ID, em formato de tabela,
        // respeitando um limite maximo de linhas informado pelo usuario.
        case 4: {
            long long limite = lerLongLong("Quantas linhas imprimir? ");
            long long cont = 0;
            printf("\n");
            imprimirEmOrdemTabela234(raiz, &cont, limite);
            printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
            printf("Linhas impressas: %lld\n", cont);
            pausar();
            break;
        }

        // Opcao 5: exibe estatisticas da arvore (altura, numero de nos, chaves, media).
        case 5: {
            printf("\n=== Estatisticas ===\n");
            printf("  Altura          : %d\n", altura234(raiz));
            printf("  Nos totais      : %d\n", contarNos234(raiz));
            printf("  Chaves totais   : %d\n", contarChaves234(raiz));
            if (contarNos234(raiz) > 0)
                printf("  Media chaves/no : %.2f\n",
                       (double)contarChaves234(raiz) / contarNos234(raiz));
            pausar();
            break;
        }

        // Opcao 6: liberta toda a memoria da arvore, deixando-a vazia.
        case 6:
            liberarArvore234(raiz);
            raiz = NULL;
            total_carregado = 0;
            printf("\n  Arvore liberada.\n");
            pausar();
            break;

        // Opcao 0: encerra o programa.
        case 0:
            printf("\nEncerrando...\n");
            break;

        default:
            printf("\n  Opcao invalida.\n");
            pausar();
        }

    } while (opcao != 0);

    // Liberta a memoria restante antes de terminar.
    liberarArvore234(raiz);
    return 0;
}
