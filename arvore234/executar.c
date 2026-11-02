#include "arvore234.h"
#include <locale.h>

static void menu(void) {
    printf("\n================================================================\n");
    printf("        ARVORE 2-3-4  --  SIVEP-Gripe\n");
    printf("================================================================\n");
    printf("  1. Definir tamanho maximo a carregar\n");
    printf("  2. Carregar CSV (respeitando o tamanho definido)\n");
    printf("  3. Inserir registro manualmente\n");
    printf("  4. Buscar por chave composta\n");
    printf("  5. Remover por chave composta\n");
    printf("  6. Imprimir em-ordem (tabela)\n");
    printf("  7. Estatisticas e validacao\n");
    printf("  8. Limpar arvore\n");
    printf("  0. Sair\n");
    printf("================================================================\n");
    printf("Opcao: ");
}

static void lerChave(Chave234* ch) {
    char buf[256];
    printf("  Cidade-UF (ex: SAO PAULO-SP): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf,"\r\n")]='\0';
        strncpy(ch->cidade_estado, buf, TAM_CIDADE-1);
        ch->cidade_estado[TAM_CIDADE-1]='\0';
    }
    printf("  Data registro (ex: 2023-08-20): ");
    if (fgets(buf, sizeof(buf), stdin)) {
        buf[strcspn(buf,"\r\n")]='\0';
        strncpy(ch->data_registro, buf, TAM_DATA-1);
        ch->data_registro[TAM_DATA-1]='\0';
    }
    printf("  ID: ");
    if (fgets(buf, sizeof(buf), stdin)) ch->id = atoll(buf);
}

static void pausar(void) {
    printf("\nPressione ENTER...");
    getchar();
}

// Funcao principal que controla o fluxo do programa, exibe o menu e executa as operacoes escolhidas.
int main(void) {
    setlocale(LC_ALL, "Portuguese");

    No234*  raiz            = NULL;
    long long limite_carga  = 0;   /* 0 = carregar tudo */
    long long carregados    = 0;
    char     ultimo_csv[256]= "";

    printf("================================================================\n");
    printf("   ARVORE 2-3-4 -- Chave composta (cidade, data, id)\n");
    printf("================================================================\n");
    printf("Dica: defina o tamanho (opcao 1) antes de carregar.\n");

    int op;
    do {
        menu();
        int leitura_valida = scanf("%d", &op) == 1;
        int caractere;
        while ((caractere = getchar()) != '\n' && caractere != EOF) {
            (void)caractere;
        }
        if (!leitura_valida) op = -1;

        switch (op) {

        case 1: {
            printf("Quantos registros carregar? (0 = todos): ");
            if (scanf("%lld", &limite_carga) != 1 || limite_carga < 0) {
                limite_carga = 0;
                printf("  Valor invalido, usando 0 (todos).\n");
            }
            while (getchar() != '\n');
            printf("  Tamanho maximo definido: %lld\n", limite_carga);
            pausar();
            break;
        }

        case 2: {
            char arq[256];
            printf("Nome do CSV (Enter para '%s'): ",
                   ultimo_csv[0] ? ultimo_csv : "INFLUD19-23-03-2026.csv");
            if (fgets(arq, sizeof(arq), stdin)) arq[strcspn(arq,"\r\n")]='\0';
            if (arq[0] == '\0') {
                if (ultimo_csv[0]) strcpy(arq, ultimo_csv);
                else strcpy(arq, "INFLUD19-23-03-2026.csv");
            }
            strncpy(ultimo_csv, arq, sizeof(ultimo_csv)-1);

            printf("Carregando '%s' (limite=%lld)...\n",
                   arq, limite_carga);

            RegistroPaciente* regs = NULL;
            long long n = lerCSV234(arq, &regs, limite_carga);
            if (n == 0) {
                printf("Nenhum registro valido carregado.\n");
                free(regs);
                pausar();
                break;
            }

            liberarArvore234(raiz);
            raiz = NULL;
            resetMetricas234();
            for (long long i = 0; i < n; i++)
                inserirNo234(&raiz, regs[i]);
            carregados = n;
            free(regs);

            printf("\n=== Carga concluida ===\n");
            printf("  Registros      : %lld\n", carregados);
            printf("  Nos            : %d\n",   contarNos234(raiz));
            printf("  Chaves         : %d\n",   contarChaves234(raiz));
            printf("  Altura         : %d\n",   altura234(raiz));
            printf("  Comparacoes    : %lld\n", g_metricas234.comparacoes);
            printf("  Splits         : %lld\n", g_metricas234.splits);
            printf("  Insercoes OK   : %lld\n", g_metricas234.insercoes_ok);
            printf("  Duplicatas     : %lld\n", g_metricas234.insercoes_dup);
            printf("  Memoria (aprox): %.2f KB\n",
                   estimarMemoria234(raiz) / 1024.0);
            pausar();
            break;
        }

        case 3: {
            RegistroPaciente r;
            memset(&r, 0, sizeof(r));
            lerChave(&r.chave);
            printf("  Sexo (M/F): ");
            if (fgets(r.sexo, sizeof(r.sexo), stdin))
                r.sexo[strcspn(r.sexo,"\r\n")]='\0';
            printf("  Nascimento: ");
            if (fgets(r.data_nascimento, sizeof(r.data_nascimento), stdin))
                r.data_nascimento[strcspn(r.data_nascimento,"\r\n")]='\0';
            printf("  Desfecho  : ");
            if (fgets(r.desfecho, sizeof(r.desfecho), stdin))
                r.desfecho[strcspn(r.desfecho,"\r\n")]='\0';

            resetMetricas234();
            if (inserirNo234(&raiz, r))
                printf("  Inserido. (comp=%lld, splits=%lld)\n",
                       g_metricas234.comparacoes, g_metricas234.splits);
            else
                printf("  Duplicata rejeitada (chave ja existente).\n");
            pausar();
            break;
        }

        case 4: {
            Chave234 ch;
            lerChave(&ch);
            resetMetricas234();
            No234* no = buscar234(raiz, &ch);
            printf("  Resultado: %s (comparacoes=%lld)\n",
                   no ? "ENCONTRADO" : "NAO ENCONTRADO",
                   g_metricas234.comparacoes);
            if (!no) {
                if (raiz == NULL)
                    printf("  Motivo: a arvore esta vazia; carregue o CSV ou insira um registro.\n");
                else
                    printf("  Motivo: a busca exige coincidencia exata de cidade-UF, data de registro e ID.\n");
            }
            if (no) {
                for (int i = 0; i < no->numChaves; i++) {
                    if (compararChaves(&no->chaves[i].chave, &ch) == 0) {
                        printf("  Desfecho: %s\n", no->chaves[i].desfecho);
                        printf("  Sexo    : %s\n", no->chaves[i].sexo);
                        printf("  Nasc.   : %s\n", no->chaves[i].data_nascimento);
                        break;
                    }
                }
            }
            pausar();
            break;
        }

        case 5: {
            Chave234 ch;
            lerChave(&ch);
            resetMetricas234();
            if (removerNo234(&raiz, &ch))
                printf("  Removido. (comparacoes=%lld)\n",
                       g_metricas234.comparacoes);
            else
                printf("  Chave nao encontrada (arvore inalterada).\n");
            ResultadoValidacao234 v = validar234(raiz);
            printf("  Validacao apos remocao: %s\n",
                   v.ok ? "OK" : v.mensagem);
            pausar();
            break;
        }

        case 6: {
            long long lim;
            printf("Limite de linhas (0 = todas): ");
            if (scanf("%lld", &lim) != 1) lim = 40;
            while (getchar() != '\n');
            if (lim <= 0) lim = (long long)1 << 60;

            long long c = 0;
            printf("\n");
            imprimirEmOrdemTabela234(raiz, &c, lim);
            printf("+---------------------+------------+----------------------+----------------+------+----------------------+\n");
            printf("Linhas impressas: %lld\n", c);
            pausar();
            break;
        }

        case 7: {
            printf("\n=== Estatisticas ===\n");
            printf("  Altura          : %d\n", altura234(raiz));
            printf("  Nos             : %d\n", contarNos234(raiz));
            printf("  Chaves          : %d\n", contarChaves234(raiz));
            printf("  Memoria (aprox) : %.2f KB\n",
                   estimarMemoria234(raiz) / 1024.0);
            if (contarNos234(raiz) > 0)
                printf("  Media chaves/no : %.2f\n",
                       (double)contarChaves234(raiz) / contarNos234(raiz));
            ResultadoValidacao234 v = validar234(raiz);
            printf("  Validacao       : %s\n",
                   v.ok ? "OK" : "FALHOU");
            if (!v.ok) printf("    %s\n", v.mensagem);
            printf("  Altura das folhas: %d\n", v.alturaFolhas);
            pausar();
            break;
        }

        case 8:
            liberarArvore234(raiz);
            raiz = NULL;
            carregados = 0;
            printf("  Arvore liberada.\n");
            pausar();
            break;

        case 0:
            printf("Encerrando...\n");
            break;

        default:
            printf("  Opcao invalida.\n");
            pausar();
        }

    } while (op != 0);

    liberarArvore234(raiz);
    return 0;
}