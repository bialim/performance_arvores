#include "../cronometro.h"
#include "arvoreAVL.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void menu(void) {
    printf("\n================================================================\n");
    printf("        ARVORE AVL  --  SIVEP-Gripe\n");
    printf("================================================================\n");
    printf("  1. Definir tamanho maximo a carregar\n");
    printf("  2. Carregar CSV (respeitando o tamanho definido)\n");
    printf("  3. Inserir registro manualmente\n");
    printf("  4. Buscar por chave composta\n");
    printf("  5. Remover por chave composta\n");
    printf("  6. Imprimir em-ordem (tabela)\n");
    printf("  7. Estatisticas e validacao\n");
    printf("  8. Limpar arvore\n");
    printf("  9. Imprimir raiz\n");
    printf("  0. Sair\n");
    printf("================================================================\n");
    printf("Opcao: ");
}

static void descartarRestoLinha(void) {
    int caractere;
    while ((caractere = getchar()) != '\n' && caractere != EOF) {}
}

static void pausar(void) {
    printf("\nPressione ENTER...");
    getchar();
}

static bool lerTexto(const char* prompt, char* destino, size_t tamanho) {
    printf("%s", prompt);
    if (fgets(destino, (int)tamanho, stdin) == NULL) return false;
    destino[strcspn(destino, "\r\n")] = '\0';
    return true;
}

static bool lerId(long long* id) {
    char buffer[64];
    if (!lerTexto("  ID: ", buffer, sizeof(buffer))) return false;
    char* fim;
    long long valor = strtoll(buffer, &fim, 10);
    if (fim == buffer || *fim != '\0' || valor <= 0) return false;
    *id = valor;
    return true;
}

static bool lerChave(ChaveAVL* chave) {
    memset(chave, 0, sizeof(*chave));
    return lerTexto("  Cidade-UF: ", chave->cidade_estado,
                    sizeof(chave->cidade_estado)) &&
           lerTexto("  Data registro: ", chave->data_registro,
                    sizeof(chave->data_registro)) &&
           lerId(&chave->id);
}

static void imprimirRegistro(const RegistroPaciente* registro) {
    printf("  ID          : %lld\n", registro->id);
    printf("  Registro    : %s\n", registro->data_registro);
    printf("  Cidade/UF   : %s\n", registro->cidade_estado);
    printf("  Sexo        : %s\n", registro->sexo);
    printf("  Nascimento  : %s\n", registro->data_nascimento);
    printf("  Desfecho    : %s\n", registro->desfecho);
}

int main(void) {
    NoAVL* raiz = NULL;
    long long limiteCarga = 0;
    long long carregados = 0;
    char ultimoCsv[256] = "";

    printf("================================================================\n");
    printf("   ARVORE AVL -- Chave por ID do paciente\n");
    printf("================================================================\n");
    printf("Dica: defina o tamanho (opcao 1) antes de carregar.\n");

    int opcao = -1;
    do {
        menu();
        if (scanf("%d", &opcao) != 1) {
            opcao = -1;
            if (feof(stdin)) break;
        }
        descartarRestoLinha();

        switch (opcao) {
        case 1:
            printf("Quantos registros carregar? (0 = todos): ");
            if (scanf("%lld", &limiteCarga) != 1 || limiteCarga < 0) {
                limiteCarga = 0;
                printf("  Valor invalido, usando 0 (todos).\n");
            }
            descartarRestoLinha();
            printf("  Tamanho maximo definido: %lld\n", limiteCarga);
            pausar();
            break;

        case 2: {
            char arquivo[256];
            char padrao[] = "INFLUD19-23-03-2026.csv";
            char prompt[320];
            snprintf(prompt, sizeof(prompt), "Nome do CSV (Enter para '%s'): ", ultimoCsv[0] ? ultimoCsv : padrao);
            if (!lerTexto(prompt, arquivo, sizeof(arquivo))) break;
            if (arquivo[0] == '\0') snprintf(arquivo, sizeof(arquivo), "%s", ultimoCsv[0] ? ultimoCsv : padrao);
            snprintf(ultimoCsv, sizeof(ultimoCsv), "%s", arquivo);

            printf("Carregando '%s' (limite=%lld)...\n", arquivo, limiteCarga);
            RegistroPaciente* registros = NULL;
            Cronometro inicioCarga = iniciarCronometro();
            long long total = lerCSVAVL(arquivo, &registros, limiteCarga);
            if (total == 0) {
                imprimirTempo("para carregar o CSV", tempoDecorridoNs(inicioCarga));
                printf("Nenhum registro valido carregado.\n");
                free(registros);
                pausar();
                break;
            }

            liberarArvoreAVL(raiz);
            raiz = NULL;
            resetMetricasAVL();
            for (long long i = 0; i < total; i++) raiz = inserirAVL(raiz, registros[i]);
            long long tempoCarga = tempoDecorridoNs(inicioCarga);
            carregados = g_metricasAVL.insercoes_ok;
            free(registros);

            printf("\n=== Carga concluida ===\n");
            printf("  Registros validos: %lld\n", total);
            printf("  Nos              : %d\n", contarNosAVL(raiz));
            printf("  Altura           : %d\n", altura(raiz));
            printf("  Comparacoes      : %lld\n", g_metricasAVL.comparacoes);
            printf("  Rotacoes         : %lld\n", g_metricasAVL.rotacoes);
            printf("  Insercoes OK     : %lld\n", g_metricasAVL.insercoes_ok);
            printf("  Duplicatas       : %lld\n", g_metricasAVL.insercoes_dup);
            printf("  Memoria (aprox)  : %.2f KB\n", estimarMemoriaAVL(raiz) / 1024.0);
            imprimirTempo("para carregar o CSV e montar a AVL", tempoCarga);
            pausar();
            break;
        }

        case 3: {
            RegistroPaciente registro = {0};
            if (!lerId(&registro.id)) {
                printf("  ID invalido.\n");
                pausar();
                break;
            }
            lerTexto("  Data registro: ", registro.data_registro, sizeof(registro.data_registro));
            lerTexto("  Cidade-UF    : ", registro.cidade_estado, sizeof(registro.cidade_estado));
            lerTexto("  Sexo (M/F)   : ", registro.sexo, sizeof(registro.sexo));
            lerTexto("  Nascimento   : ", registro.data_nascimento, sizeof(registro.data_nascimento));
            lerTexto("  Desfecho     : ", registro.desfecho, sizeof(registro.desfecho));
            resetMetricasAVL();
            Cronometro inicioInsercao = iniciarCronometro();
            raiz = inserirAVL(raiz, registro);
            long long tempoInsercao = tempoDecorridoNs(inicioInsercao);
            if (g_metricasAVL.insercoes_ok)
                printf("  Inserido. (comparacoes=%lld, rotacoes=%lld)\n", g_metricasAVL.comparacoes, g_metricasAVL.rotacoes);
            else
                printf("  Duplicata rejeitada (ID ja existente).\n");
            imprimirTempo("para inserir", tempoInsercao);
            carregados = contarNosAVL(raiz);
            pausar();
            break;
        }

        case 4: {
            ChaveAVL chave;
            if (!lerChave(&chave)) {
                printf("  Chave composta invalida.\n");
                pausar();
                break;
            }
            resetMetricasAVL();
            Cronometro inicioBusca = iniciarCronometro();
            NoAVL* encontrado = buscarAVL(raiz, &chave);
            long long tempoBusca = tempoDecorridoNs(inicioBusca);
            printf("  Resultado: %s (comparacoes=%lld)\n", encontrado ? "ENCONTRADO" : "NAO ENCONTRADO", g_metricasAVL.comparacoes);
            imprimirTempo("para buscar", tempoBusca);
            if (encontrado != NULL) imprimirRegistro(&encontrado->dado);
            else if (raiz == NULL) printf("  Motivo: a arvore esta vazia.\n");
            pausar();
            break;
        }

        case 5: {
            ChaveAVL chave;
            if (!lerChave(&chave)) {
                printf("  Chave composta invalida.\n");
                pausar();
                break;
            }
            resetMetricasAVL();
            Cronometro inicioRemocao = iniciarCronometro();
            bool removido = removerAVL(&raiz, &chave);
            long long tempoRemocao = tempoDecorridoNs(inicioRemocao);
            if (removido) printf("  Removido. (comparacoes=%lld, rotacoes=%lld)\n", g_metricasAVL.comparacoes, g_metricasAVL.rotacoes);
            else printf("  ID nao encontrado (arvore inalterada).\n");
            imprimirTempo("para remover", tempoRemocao);
            carregados = contarNosAVL(raiz);
            ResultadoValidacaoAVL validacao = validarAVL(raiz);
            printf("  Validacao apos remocao: %s\n", validacao.ok ? "OK" : validacao.mensagem);
            pausar();
            break;
        }

        case 6: {
            long long limite;
            printf("Limite de linhas (0 = todas): ");
            if (scanf("%lld", &limite) != 1) limite = 0;
            descartarRestoLinha();
            if (limite <= 0) limite = LLONG_MAX;
            long long contador = 0;
            printf("\n");
            imprimirEmOrdemTabela(raiz, &contador, limite);
            printf("+----------------+--------------+--------------+---------------------------+------+----------------------+\n");
            printf("Linhas impressas: %lld\n", contador);
            pausar();
            break;
        }

        case 7: {
            ResultadoValidacaoAVL validacao = validarAVL(raiz);
            printf("\n=== Estatisticas ===\n");
            printf("  Altura          : %d\n", altura(raiz));
            printf("  Nos             : %d\n", contarNosAVL(raiz));
            printf("  Registros ativos: %lld\n", carregados);
            printf("  Memoria (aprox) : %.2f KB\n", estimarMemoriaAVL(raiz) / 1024.0);
            printf("  Validacao       : %s\n", validacao.ok ? "OK" : "FALHOU");
            if (!validacao.ok) printf("    %s\n", validacao.mensagem);
            pausar();
            break;
        }

        case 8:
            liberarArvoreAVL(raiz);
            raiz = NULL;
            carregados = 0;
            printf("  Arvore liberada.\n");
            pausar();
            break;

        case 9:
            if (raiz == NULL) {
                printf("  A arvore esta vazia.\n");
            } else {
                printf("\n=== Raiz da arvore AVL ===\n");
                imprimirRegistro(&raiz->dado);
            }
            pausar();
            break;

        case 0:
            printf("Encerrando...\n");
            break;

        default:
            printf("  Opcao invalida.\n");
            pausar();
        }
    } while (opcao != 0);

    liberarArvoreAVL(raiz);
    return 0;
}