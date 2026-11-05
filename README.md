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
Ministério da Saúde - Base de Dados SIVEP-Gripe (INFLUD19), obtida através de planilha disponibilizada no portal de **Dados Abertos do Ministério da Saúde** (https://dadosabertos.saude.gov.br/dataset/srag-2019-a-2026). O conjunto de dados é o "Banco de dados da Síndrome Respiratória Aguda Grave (SRAG) - 2019 a 2026", o arquivo específico utilizado neste projeto é o `INFLUD19-23-03-2026.csv`.

### 1.3 Estrutura dos Dados
A chave de busca e ordenação é composta por `cidade_estado`, `data_registro` e `id`, comparados nessa ordem. Os demais dados do paciente são armazenados como informações associadas ao registro:
- `sexo` *(string)*: Sexo informado no registro.
- `data_nascimento` *(string)*: Data de nascimento.
- `desfecho` *(string)*: Evolução do caso.
- 
### 1.4 Justificativa da Escolha
O DATASET SRAG/SIVEP-Gripe é adequado para testar estruturas de dados complexas, devido ao volume considerável de registros (quase 50 mil) e à utilização de identificadores (data de registro e cidade-estado) na forma de números inteiros longos. A necessidade de ordenar rapidamente esses IDs longos e associá-los a informações clínicas complexas simula perfeitamente o ambiente de um banco de dados real da área da saúde, exigindo alta performance tanto na inserção quanto na busca.

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
- [x] Inserção
- [x] Remoção
- [x] Busca
- [x] Percursos (Pré-ordem, Em ordem, Pós-ordem)
- [x] Balanceamento *(se aplicável)*
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
| 4 | Performance de Integridade | Compilação da árvore 50 vezes consecutivas após inicializar a máquina. | A compilação de ambas as árvores deve registrar um tempo de processamento minimizado, com variação quase nula entre as 50 iterações. | [ x ] |


### 3.3 Casos Extremos (Edge Cases)
Além dos testes de desempenho, serão verificados casos que podem causar erros de implementação.

**Árvore vazia:** a busca e a remoção de uma chave inexistente devem terminar corretamente, sem acessar ponteiros inválidos.

**Árvore com um único registro:** a busca deve localizar a chave; a remoção deve deixar a árvore vazia.

**Chave duplicada:** a segunda inserção da mesma chave composta deve ser rejeitada nas duas árvores.

**Remoção da raiz:** após a operação, a árvore deve manter sua ordenação e seu balanceamento.

**Remoção de chave inexistente:** a operação deve informar que a chave não foi encontrada, sem modificar a estrutura.

**Remoção completa:** após remover todos os registros, a estrutura deve estar vazia e apta a receber novas chaves.

### 3.4 Testes de Desempenho *(Para Entrega 2)*
Os testes serão realizados com conjuntos de tamanhos crescentes, utilizando os mesmos registros e a mesma ordem de entrada para as duas árvores.

Os tamanhos efetivamente testados foram:

- 1.000 registros;
- 10.000 registros;
- 25.000 registros;
- 48.941 registros válidos, correspondentes a todos os registros utilizados do arquivo CSV.

Para cada tamanho, serão medidos separadamente:

- Tempo de inserção de todos os registros.
- Tempo de busca de um conjunto fixo de chaves existentes e inexistentes.
- Tempo de remoção de um conjunto fixo de chaves existentes.
- Altura final da árvore.
- Quantidade de comparações realizadas.
- Quantidade de rotações da AVL.
- Quantidade de splits da árvore 2-3-4.
- Estimativa de memória ocupada pela estrutura.

As medições deverão separar o tempo de leitura e interpretação do CSV do tempo gasto pelas operações das árvores. Dessa forma, será possível comparar o custo das estruturas sem misturá-lo ao custo de entrada e saída de arquivos.

### 3.5 Resultados Obtidos *(Para Entrega 2)*
Foram realizados testes de carga e busca nas duas estruturas com 1.000, 10.000, 25.000 e 48.941 registros válidos. Em todos os testes de carga apresentados, a quantidade de inserções bem-sucedidas correspondeu à quantidade de registros carregados, e não foram registradas duplicatas.
### 3.5.1 Resultados da árvore AVL

| Registros | Nós | Altura | Comparações na inserção | Rotações | Memória aproximada (KB) | Tempo de carga (ms) |
|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 1.000 | 12 | 8.761 | 717 | 213,34 | 16,694000 |
| 10.000 | 10.000 | 16 | 120.954 | 7.144 | 2.097,16 | 74,085958 |
| 25.000 | 25.000 | 18 | 335.897 | 17.616 | 5.458,84 | 202,203958 |
| 48.941 | 48.941 | 19 | 706.174 | 34.341 | 10.710,82 | 371,657916 |

### 3.5.2 Resultados da árvore 2-3-4

| Registros | Nós | Chaves | Altura | Comparações na inserção | Splits | Memória aproximada (KB) | Tempo de carga (ms) |
|---:|---:|---:|---:|---:|---:|---:|---:|
| 1.000 | 577 | 1.000 | 8 | 8.893 | 569 | 333,59 | 10,047916 |
| 10.000 | 5.731 | 10.000 | 11 | 122.959 | 5.720 | 3.278,18 | 118,014166 |
| 25.000 | 14.248 | 25.000 | 12 | 341.557 | 14.236 | 8.367,91 | 207,817708 |
| 48.941 | 27.927 | 48.941 | 13 | 717.766 | 27.914 | 16.424,62 | 353,742375 |

### 3.5.3 Resultados das buscas

No teste com 1.000 registros, as duas implementações pesquisaram registros diferentes. Por isso, esse caso é apresentado separadamente e não deve ser considerado uma comparação direta.

| Registros | Estrutura | Chave pesquisada (cidade/UF, data, ID) | Resultado | Comparações | Tempo (ms) |
|---:|---|---|---|---:|---:|
| 1.000 | AVL | ALTAMIRA-PA, 2019-02-04, 315592350125 | Encontrado | 10 | 0,003416 |
| 1.000 | 2-3-4 | ALEM PARAIBA-MG, 2019-07-12, 315631909307 | Encontrado | 8 | 0,004583 |
| 10.000 | AVL | -, 2019-01-11, 315493734059 | Encontrado | 13 | 0,010458 |
| 10.000 | 2-3-4 | -, 2019-01-11, 315493734059 | Encontrado | 11 | 0,055166 |
| 25.000 | AVL | -, 2019-01-11, 315493734059 | Encontrado | 15 | 0,133958 |
| 25.000 | 2-3-4 | -, 2019-01-11, 315493734059 | Encontrado | 12 | 0,024875 |
| 48.941 | AVL | -, 2019-01-11, 315493734059 | Encontrado | 15 | 0,033958 |
| 48.941 | 2-3-4 | -, 2019-01-11, 315493734059 | Encontrado | 12 | 0,074666 |

Nos testes de busca com 10.000, 25.000 e 48.941 registros, foi utilizada a mesma chave nas duas árvores. O registro encontrado apresenta data de registro 2019-01-11, ID 315493734059, sexo masculino, nascimento em 2010-09-08 e desfecho Cura (2019-01-21).

---

## 4. Como Executar

### 4.1 Pré-requisitos *(Para Entrega 2)*
-   Linguagem C
-   Compilador C (GCC / MinGW)
-   Ambiente sugerido: Code::Blocks ou VSCode

### 4.2 Instruções de Execução *(Para Entrega 2)*

projeto/
├── arvore234/
│   ├── arvore234.c
│   ├── arvore234.h
│   ├── executar.c
│   ├── INFLUD19-23-03-2026.csv
│   └── output/
│
├── arvoreAVL/
│   ├── arvoreAVL.c
│   ├── arvoreAVL.h
│   ├── executar.c
│   ├── INFLUD19-23-03-2026.csv
│   └── output/
│
├── cronometro.h
└── README.md

---

## 5. Referências
- Portal de Dados Abertos do Ministério da Saúde. Disponível em: https://dadosabertos.saude.gov.br/dataset/srag-2019-a-2026. /br
- Documentação do SIVEP-Gripe (Ministério da Saúde - Brasil).
- Material Didático - Disciplina Estruturas de Dados II, UNICID.
