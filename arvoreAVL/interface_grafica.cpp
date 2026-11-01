#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ============================================================================
// 1. ESTRUTURAS E LÓGICA DA ÁRVORE AVL
// ============================================================================

typedef struct {
    long long id;
    char data_registro[15];
    char cidade_estado[80];
    char sexo[5];
    char data_nascimento[15];
    char desfecho[35];
} RegistroPaciente;

typedef struct NoAVL {
    RegistroPaciente dado;
    int altura;
    struct NoAVL *esquerda;
    struct NoAVL *direita;
} NoAVL;

int altura(NoAVL *N) {
    if (N == NULL) return 0;
    return N->altura;
}

int max_val(int a, int b) { return (a > b) ? a : b; }

NoAVL* novoNo(RegistroPaciente dado) {
    NoAVL* no = (NoAVL*)malloc(sizeof(NoAVL));
    no->dado = dado;
    no->esquerda = NULL;
    no->direita = NULL;
    no->altura = 1;
    return no;
}

NoAVL* rotacaoDireita(NoAVL *y) {
    NoAVL *x = y->esquerda;
    NoAVL *T2 = x->direita;
    x->direita = y;
    y->esquerda = T2;
    y->altura = max_val(altura(y->esquerda), altura(y->direita)) + 1;
    x->altura = max_val(altura(x->esquerda), altura(x->direita)) + 1;
    return x;
}

NoAVL* rotacaoEsquerda(NoAVL *x) {
    NoAVL *y = x->direita;
    NoAVL *T2 = y->esquerda;
    y->esquerda = x;
    x->direita = T2;
    x->altura = max_val(altura(x->esquerda), altura(x->direita)) + 1;
    y->altura = max_val(altura(y->esquerda), altura(y->direita)) + 1;
    return y;
}

int obterBalanceamento(NoAVL *N) {
    if (N == NULL) return 0;
    return altura(N->esquerda) - altura(N->direita);
}

NoAVL* inserirAVL(NoAVL* no, RegistroPaciente dado) {
    if (no == NULL) return novoNo(dado);
    if (dado.id < no->dado.id) no->esquerda = inserirAVL(no->esquerda, dado);
    else if (dado.id > no->dado.id) no->direita = inserirAVL(no->direita, dado);
    else return no; // IDs iguais não são permitidos na AVL básica

    no->altura = 1 + max_val(altura(no->esquerda), altura(no->direita));
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

// --- FUNÇÕES NOVAS ADICIONADAS PARA A INTERFACE ---

NoAVL* buscarAVL(NoAVL* raiz, long long id) {
    if (raiz == NULL || raiz->dado.id == id) return raiz;
    if (raiz->dado.id < id) return buscarAVL(raiz->direita, id);
    return buscarAVL(raiz->esquerda, id);
}

NoAVL* noMinimo(NoAVL* no) {
    NoAVL* atual = no;
    while (atual->esquerda != NULL) atual = atual->esquerda;
    return atual;
}

NoAVL* removerAVL(NoAVL* raiz, long long id) {
    if (raiz == NULL) return raiz;

    if (id < raiz->dado.id) raiz->esquerda = removerAVL(raiz->esquerda, id);
    else if (id > raiz->dado.id) raiz->direita = removerAVL(raiz->direita, id);
    else {
        if ((raiz->esquerda == NULL) || (raiz->direita == NULL)) {
            NoAVL *temp = raiz->esquerda ? raiz->esquerda : raiz->direita;
            if (temp == NULL) {
                temp = raiz;
                raiz = NULL;
            } else {
                *raiz = *temp;
            }
            free(temp);
        } else {
            NoAVL* temp = noMinimo(raiz->direita);
            raiz->dado = temp->dado;
            raiz->direita = removerAVL(raiz->direita, temp->dado.id);
        }
    }

    if (raiz == NULL) return raiz;

    raiz->altura = 1 + max_val(altura(raiz->esquerda), altura(raiz->direita));
    int balanceamento = obterBalanceamento(raiz);

    if (balanceamento > 1 && obterBalanceamento(raiz->esquerda) >= 0) return rotacaoDireita(raiz);
    if (balanceamento > 1 && obterBalanceamento(raiz->esquerda) < 0) {
        raiz->esquerda = rotacaoEsquerda(raiz->esquerda);
        return rotacaoDireita(raiz);
    }
    if (balanceamento < -1 && obterBalanceamento(raiz->direita) <= 0) return rotacaoEsquerda(raiz);
    if (balanceamento < -1 && obterBalanceamento(raiz->direita) > 0) {
        raiz->direita = rotacaoDireita(raiz->direita);
        return rotacaoEsquerda(raiz);
    }
    return raiz;
}

void liberarArvoreAVL(NoAVL* raiz) {
    if (raiz != NULL) {
        liberarArvoreAVL(raiz->esquerda);
        liberarArvoreAVL(raiz->direita);
        free(raiz);
    }
}

// Leitura simplificada do CSV original
void pegar_coluna_por_indice(const char *linha, int coluna_desejada, char *resultado) {
    int col_atual = 0;
    int i = 0, j = 0;
    resultado[0] = '\0';
    while (linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
        if (col_atual == coluna_desejada) {
            while (linha[i] != ';' && linha[i] != '\0' && linha[i] != '\n' && linha[i] != '\r') {
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

NoAVL* carregarPlanilhaCSV(const char* nomeArquivo, NoAVL* raiz, int *total_carregado) {
    FILE* arquivo = fopen(nomeArquivo, "r");
    if (!arquivo) return raiz;

    char linha[30000];
    char buffer_temp[255], estado[10], cidade[90];
    fgets(linha, sizeof(linha), arquivo); // Pula cabeçalho

    while (fgets(linha, sizeof(linha), arquivo) && *total_carregado < 40) {
        RegistroPaciente reg;
        memset(&reg, 0, sizeof(RegistroPaciente));

        pegar_coluna_por_indice(linha, 0, buffer_temp);
        reg.id = atoll(buffer_temp);
        if (reg.id == 0) continue;

        pegar_coluna_por_indice(linha, 5, estado);
        pegar_coluna_por_indice(linha, 8, cidade);
        sprintf(reg.cidade_estado, "%s-%s", cidade, estado);

        pegar_coluna_por_indice(linha, 10, reg.sexo);
        strcpy(reg.desfecho, "CSV Carregado");

        raiz = inserirAVL(raiz, reg);
        (*total_carregado)++;
    }
    fclose(arquivo);
    return raiz;
}

// ============================================================================
// 2. INTERFACE GRÁFICA (GTK3)
// ============================================================================

NoAVL* arvoreRaiz = NULL;

GtkWidget *entry_id, *entry_cidade, *entry_sexo, *label_status;

void set_status(const char* msg) {
    gtk_label_set_text(GTK_LABEL(label_status), msg);
}

void on_btn_carregar_clicked(GtkButton *button, gpointer user_data) {
    int carregados = 0;
    arvoreRaiz = carregarPlanilhaCSV("INFLUD19-23-03-2026.csv", arvoreRaiz, &carregados);
    char msg[100];
    if (carregados > 0) sprintf(msg, "%d registros carregados do CSV!", carregados);
    else sprintf(msg, "Erro: Arquivo CSV não encontrado na pasta.");
    set_status(msg);
}

void on_btn_inserir_clicked(GtkButton *button, gpointer user_data) {
    long long id = atoll(gtk_entry_get_text(GTK_ENTRY(entry_id)));
    if (id <= 0) {
        set_status("Erro: Insira um ID válido!");
        return;
    }
    RegistroPaciente reg;
    memset(&reg, 0, sizeof(RegistroPaciente));
    reg.id = id;
    strncpy(reg.cidade_estado, gtk_entry_get_text(GTK_ENTRY(entry_cidade)), 79);
    strncpy(reg.sexo, gtk_entry_get_text(GTK_ENTRY(entry_sexo)), 4);
    strcpy(reg.desfecho, "Inserção Manual");

    arvoreRaiz = inserirAVL(arvoreRaiz, reg);
    char msg[100];
    sprintf(msg, "Sucesso: Paciente %lld inserido na AVL!", id);
    set_status(msg);
}

void on_btn_buscar_clicked(GtkButton *button, gpointer user_data) {
    long long id = atoll(gtk_entry_get_text(GTK_ENTRY(entry_id)));
    NoAVL* res = buscarAVL(arvoreRaiz, id);
    if (res) {
        char msg[256];
        sprintf(msg, "ENCONTRADO -> ID: %lld | Cidade: %s | Sexo: %s",
                res->dado.id, res->dado.cidade_estado, res->dado.sexo);
        set_status(msg);
    } else {
        set_status("Paciente não encontrado na árvore.");
    }
}

void on_btn_remover_clicked(GtkButton *button, gpointer user_data) {
    long long id = atoll(gtk_entry_get_text(GTK_ENTRY(entry_id)));
    if (buscarAVL(arvoreRaiz, id) != NULL) {
        arvoreRaiz = removerAVL(arvoreRaiz, id);
        char msg[100];
        sprintf(msg, "Paciente %lld removido (Árvore rebalanceada).", id);
        set_status(msg);
    } else {
        set_status("Erro: ID não existe para remover.");
    }
}

void on_btn_desalocar_clicked(GtkButton *button, gpointer user_data) {
    liberarArvoreAVL(arvoreRaiz);
    arvoreRaiz = NULL;
    set_status("Árvore destruída. Toda a memória foi desalocada.");
}

static void activate(GtkApplication *app, gpointer user_data) {
    GtkWidget *window, *grid;
    GtkWidget *l_id, *l_cid, *l_sex;
    GtkWidget *btn_load, *btn_ins, *btn_bus, *btn_rem, *btn_des;

    window = gtk_application_window_new(app);
    gtk_window_set_title(GTK_WINDOW(window), "SIVEP - Árvore AVL");
    gtk_window_set_default_size(GTK_WINDOW(window), 450, 250);
    gtk_container_set_border_width(GTK_CONTAINER(window), 15);

    grid = gtk_grid_new();
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_container_add(GTK_CONTAINER(window), grid);

    // Labels e Entries
    l_id = gtk_label_new("ID Paciente:");
    entry_id = gtk_entry_new();
    l_cid = gtk_label_new("Cidade-UF:");
    entry_cidade = gtk_entry_new();
    l_sex = gtk_label_new("Sexo (M/F):");
    entry_sexo = gtk_entry_new();

    // Botões
    btn_load = gtk_button_new_with_label("Carregar CSV");
    btn_ins = gtk_button_new_with_label("Inserir");
    btn_bus = gtk_button_new_with_label("Buscar");
    btn_rem = gtk_button_new_with_label("Remover");
    btn_des = gtk_button_new_with_label("Desalocar Tudo");

    label_status = gtk_label_new("Pronto.");

    // Anexando ao Grid
    gtk_grid_attach(GTK_GRID(grid), l_id, 0, 0, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_id, 1, 0, 3, 1);
    gtk_grid_attach(GTK_GRID(grid), l_cid, 0, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_cidade, 1, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), l_sex, 2, 1, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), entry_sexo, 3, 1, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), btn_load, 0, 2, 4, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_ins, 0, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_bus, 1, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_rem, 2, 3, 1, 1);
    gtk_grid_attach(GTK_GRID(grid), btn_des, 3, 3, 1, 1);

    gtk_grid_attach(GTK_GRID(grid), label_status, 0, 4, 4, 1);

    // Conectando cliques aos callbacks
    g_signal_connect(btn_load, "clicked", G_CALLBACK(on_btn_carregar_clicked), NULL);
    g_signal_connect(btn_ins, "clicked", G_CALLBACK(on_btn_inserir_clicked), NULL);
    g_signal_connect(btn_bus, "clicked", G_CALLBACK(on_btn_buscar_clicked), NULL);
    g_signal_connect(btn_rem, "clicked", G_CALLBACK(on_btn_remover_clicked), NULL);
    g_signal_connect(btn_des, "clicked", G_CALLBACK(on_btn_desalocar_clicked), NULL);

    gtk_widget_show_all(window);
}

int main(int argc, char **argv) {
    GtkApplication *app = gtk_application_new("org.sivep.avl", G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(activate), NULL);
    int status = g_application_run(G_APPLICATION(app), argc, argv);
    g_object_unref(app);

    if (arvoreRaiz != NULL) liberarArvoreAVL(arvoreRaiz);
    return status;
}