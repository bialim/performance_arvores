# Comparação de Performance entre as Árvores AVL e 2-3-4 ao Analisar Dados Complexos de Casos de SRAG no (2019-2026)

Entrega 1 de trabalho da disciplina Estruturas de Dados II — UNICID Prof. Cid Rodrigues de Andrade </br>

---

## 👥 Integrantes do Grupo

| Nome Completo | RA |
| :--- | :--- |
| André Goi Collossal | 42500516 |
| Beatriz Lima Morais | 47359234 |
| Gustavo Gomes Cirqueira | 42934095 |
| Natalia Souza Dias | 42140897 |
| Yhemy Quiuchaca Mamani | 43073875 |

---

## 1. Dataset

### 1.1 Descrição
O conjunto de dados utilizado consiste em casos de Síndrome Respiratória Aguda Grave (SRAG) no Brasil (2019-2026), extraídos da base oficial SIVEP-Gripe do Ministério da Saúde. O arquivo encontra-se no formato CSV e contém quase 50 mil registros de pacientes. A planilha completa engloba informações sobre os pacientes e desfechos clínicos.

### 1.2 Fonte
Ministério da Saúde - Base de Dados SIVEP-Gripe (INFLUD19), obtida através de planilha disponibilizada no portal de **Dados Abertos do Ministério da Saúde** (https://dadosabertos.saude.gov.br/). O conjunto de dados é o "Banco de dados da Síndrome Respiratória Aguda Grave (SRAG) - 2019 a 2026", o arquivo específico utilizado neste projeto é o `INFLUD19-23-03-2026.csv`.

### 1.3 Estrutura dos Dados
Descreva os campos/atributos relevantes que serão usados como chave de inserção, busca e comparação nas árvores:
- `id` *(long long)*: Chave principal, identificador único do paciente.
- `data_registro` *(string)*: Data de registro da notificação.
- `cidade_estado` *(string)*: Município e Unidade Federativa combinados.
- `sexo` *(string)*: Sexo do paciente.
- `data_nascimento` *(string)*: Data de nascimento.
- `desfecho` *(string)*: Evolução do caso (Cura, Óbito, etc.) com a data respectiva.

### 1.4 Justificativa da Escolha
O DATASET SRAG/SIVEP-Gripe é adequado para testar estruturas de dados complexas, devido ao volume considerável de registros (quase 50 mil) e à utilização de identificadores (IDs) na forma de números inteiros longos. A necessidade de ordenar rapidamente esses IDs longos e associá-los a informações clínicas complexas simula perfeitamente o ambiente de um banco de dados real da área da saúde, exigindo alta performance tanto na inserção quanto na busca.

---

## 2. Estrutura(s) de Árvore Escolhida(s)

### 2.1 Estrutura(s)
Foram implementadas e serão comparadas duas estruturas de busca auto-balanceáveis:
1.  **Árvore AVL** (Adelson-Velsky e Landis)
2.  **Árvore 2-3-4** (Árvore de Pesquisa Multivias)

### 2.2 Justificativa Técnica
-   **Árvore AVL:** Foi escolhida por ser rigorosamente balanceada. Para o SIVEP-Gripe, onde consultas a pacientes específicos pelo ID precisam ser muito rápidas, a AVL garante que o pior caso de busca seja altamente eficiente, mesmo com um volume gigantesco de dados. Ela mantém o equilíbrio verificando a altura e aplicando rotações a cada inserção desbalanceada.
-   **Árvore 2-3-4:** Foi escolhida como contraponto à AVL, pois introduz o conceito de nós com múltiplos itens e filhos. Ao agrupar até 3 pacientes no mesmo nó e crescer "para cima" através de *splits* preemptivos (sem usar rotações), ela gera árvores muito mais achatadas. É excelente para comparar o desempenho de alocação de memória RAM contra simulações de acesso a disco (base teórica das B-Trees usadas em SGBDs reais).

### 2.3 Operações Implementadas *(Para Entrega 2)*
- [ ] Inserção
- [ ] Remoção
- [ ] Busca
- [ ] Percursos (Pré-ordem, Em ordem, Pós-ordem)
- [ ] Balanceamento *(se aplicável)*
- [ ] Outra: `______`

### 2.4 Complexidade
Tabela com a complexidade assintótica (Big-O) teórica de cada operação implementada:

| Operação | Melhor Caso | Caso Médio | Pior Caso |
| :--- | :---: | :---: | :---: |
| **Inserção** | $O(1)$ | $O(\log N)$ | $O(\log N)$ |
| **Busca** | $O(1)$ | $O(\log N)$ | $O(\log N)$ |
| **Remoção** | $O(1)$ |$O(\log N)$ | $O(\log N)$ |

---

## 3. Plano de Testes
Dataset de Ensaio: `INFLUD19-23-03-2026.csv` (N = 48.942 registros).

### 3.1 Objetivo dos Testes
Ambientes de Execução: Linux x86_64 / Windows 11 x86_64, Compilador GCC.

O grupo pretende validar empiricamente a integridade estrutural e comparar o desempenho (tempo e memória) das implementações em C da Árvore AVL e da Árvore 2-3-4 sobre o dataset SRAG. Os testes visam:
-   **Validar Inserções Complexas:** Garantir o correto armazenamento, extração segura do CSV (tratando delimitadores) e alinhamento dos atributos clínicos na `struct`.
-   **Comprovar Balanceamento:** Confirmar que as rotações matemáticas (AVL) e as divisões/splits (2-3-4) limitam o crescimento vertical estritamente em $O(\log N)$.
-   **Avaliar Comportamento Sob Carga:** Inserir massivamente 49 mil IDs ordenados cronologicamente (pior caso em BSTs simples) e validar qual algoritmo auto-balanceável sofre menor latência.

### 3.2 Cenários de Teste
| # | Cenário | Entrada | Resultado Esperado | Status |
| :-: | :--- | :--- | :--- | :-: |
| 1 | Carga Massiva CSV | Inserção das primeiras 40 linhas do arquivo INFLUD19-23-03-2026.csv. | Ambas as árvores criadas em memória com sucesso. A AVL deve aplicar rotações e a 2-3-4 deve agrupar nós, mantendo a estrutura balanceada. | [ x ] |
| 2 | Exibição Tabular | Percorrer as árvores utilizando o método Em-Ordem (*In-Order*). | Exibição no console de uma tabela perfeitamente alinhada, com os pacientes ordenados de forma estritamente crescente pelo ID. | [ x ] |
| 3 | Forçar Desbalanceamento | Inserção de IDs simulados em ordem estritamente crescente. | A AVL deve acionar rotações imediatas. A 2-3-4 deve realizar o *split* preventivo da raiz, absorvendo as inserções sem perder a velocidade. | [ x ] |
| 4 | Performance de Integridade | Compilação da árvore 50 vezes consecutivas após inicializar a máquina. | Ambas as árvores devem simular um tempo rápido e coeso de execução. | [ x ] |


### 3.3 Casos Extremos (Edge Cases)
-   **Dados Mal Formatados/Vazios no CSV:** A função `pegar_coluna_por_indice` foi desenvolvida para avançar sobre aspas (`"`) indesejadas e pular IDs nulos ou iguais a zero.
-   **Limites de Console Windows:** Criação de rotinas de cabeçalho dinâmico (a cada 40 linhas) para evitar o desaparecimento dos títulos das colunas após *scrolling* agressivo, e controle estrito do tamanho das *strings* para evitar quebras visuais.
-   **Crescimento da Árvore 2-3-4:** Ao contrário da AVL que cresce para baixo, validou-se que as folhas da 2-3-4 mantêm a profundidade constante, promovendo a chave intermediária ao pai em caso de colisão, dividindo o nó.

### 3.4 Testes de Desempenho *(Para Entrega 2)*
Descreva como o grupo mediu o tempo de execução e o uso de memória em diferentes tamanhos de amostragem (ex.: 100, 1.000, 10.000 e 100.000 registros).

### 3.5 Resultados Obtidos *(Para Entrega 2)*
Resuma os resultados (tabelas, gráficos ou links para arquivos em `/resultados`) e compare-os com a complexidade assintótica teórica esperada.

---

## 4. Como Executar

### 4.1 Pré-requisitos *(Para Entrega 2)*
-   Linguagem C
-   Compilador C (GCC / MinGW)
-   Ambiente sugerido: Code::Blocks ou VSCode

### 4.2 Instruções de Execução *(Para Entrega 2)*

/src         → código-fonte
/dataset     → dataset utilizado
/testes      → scripts e casos de teste
/resultados  → saídas e relatórios de desempenho
README.md

---

## 5. Referências
- Portal de Dados Abertos do Ministério da Saúde. Disponível em: https://dadosabertos.saude.gov.br/.
- Documentação do SIVEP-Gripe (Ministério da Saúde - Brasil).
- Material Didático - Disciplina Estruturas de Dados II, UNICID.
