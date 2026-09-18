# Comparação de Performance entre as Árvores AVL e 2-3-4 ao Analisar Dados Complexos da Gripe SRAG

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
Descreva o conjunto de dados utilizado nos testes: origem, domínio/assunto, formato (CSV, JSON, etc.) e volume de registros.  
*Obs.: Os dados devem ser complexos/compostos, contendo entre 50 mil e 1 milhão de registros.*

### 1.2 Fonte
Link ou referência de onde o dataset foi obtido (ex.: Kaggle, API pública, dados gerados pelo grupo, etc.).

### 1.3 Estrutura dos Dados
Descreva os campos/atributos relevantes que serão usados como chave de inserção, busca e comparação nas árvores:
- `id` *(int)*: Identificador único / Chave principal
- `nome` *(string)*: Campo descritivo
- `valor` *(float)*: Atributo numérico de exemplo

### 1.4 Justificativa da Escolha
Explique o porquê este dataset é adequado para testar a(s) estrutura(s) de árvore escolhida(s) (volume, distribuição dos dados, tipo de chave, etc.).

---

## 2. Estrutura(s) de Árvore Escolhida(s)

### 2.1 Estrutura(s)
Liste a(s) árvore(s) implementada(s) (ex.: BST, AVL, Árvore Rubro-Negra, B-Tree, etc.).

### 2.2 Justificativa Técnica
Por que essa(s) estrutura(s) foi(ram) escolhida(s) para este dataset e problema? Considere fatores como complexidade temporal, balanceamento automático e caso de uso ideal.

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
| **Inserção** |  |  | |
| **Busca** |  |  |  |
| **Remoção** |  | |  |

---

## 3. Plano de Testes

### 3.1 Objetivo dos Testes
Descreva o que o grupo pretende validar (corretude do algoritmo, desempenho, comportamento em limites do dataset, etc.).

### 3.2 Cenários de Teste

| # | Cenário | Entrada | Resultado Esperado | Status |
| :-: | :--- | :--- | :--- | :-: |
| 1 | |  |  | [ ] |
| 2 | |  | | [ ] |
| 3 |  |  | | [ ] |

### 3.3 Casos Extremos (Edge Cases)
Liste casos como: árvore vazia, único elemento, dados duplicados, dados em ordem crescente/decrescente (pior caso para BST), volume máximo do dataset, etc.


### 3.4 Testes de Desempenho *(Para Entrega 2)*
Descreva como o grupo mediu o tempo de execução e o uso de memória em diferentes tamanhos de amostragem (ex.: 100, 1.000, 10.000 e 100.000 registros).

### 3.5 Resultados Obtidos *(Para Entrega 2)*
Resuma os resultados (tabelas, gráficos ou links para arquivos em `/resultados`) e compare-os com a complexidade assintótica teórica esperada.

---

## 4. Como Executar

### 4.1 Pré-requisitos *(Para Entrega 2)*
Linguagem, versão e dependências necessárias.

### 4.2 Instruções de Execução *(Para Entrega 2)*

/src         → código-fonte
/dataset     → dataset utilizado
/testes      → scripts e casos de teste
/resultados  → saídas e relatórios de desempenho
README.md

---

## 5. Referências

Bibliografia, artigos ou materiais consultados.
