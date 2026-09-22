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
Dataset de Ensaio: INFLUD19-23-03-2026.csv (N = 48,941 registros de notificações de SRAG).

### 3.1 Objetivo dos Testes

Ambiente de Execução: Linux x86_64, Windows 11 x86_64 / Compilador GCC 13.2.0 

  O grupo pretende validar empiricamente a corretude funcional, a integridade matemática das regras de balanceamento e o desempenho temporal espacial das implementações em C da Árvore AVL e da Árvore 2-3-4 sobre o dataset real de 48,941 registos de SRAG. Especificamente, os testes visam:

Validar as Operações Fundamentais (CRUD): Garantir a precisão na inserção, pesquisa e remoção de registos através da chave composta alfanumérica.

Comprovar o Balanceamento Dinâmico: Confirmar que as rotações (AVL) e as divisões de nós (splits preemptivos na 2-3-4) mantêm a altura da árvore dentro dos limites logarítmicos O(log N).

Analisar a Tolerância a Dados Ordenados: Avaliar o comportamento de ambas as estruturas sob inserção estritamente cronológica (pior caso de inserção em BSTs simples).

Garantir a Sanidade da Memória e Persistência: Confirmar a ausência de memory leaks via Valgrind e validar a reconstrução exata da árvore após simulação de reinicialização do sistema (reboot test).


### 3.2 Cenários de Teste

| # | Cenário | Entrada | Resultado Esperado | Status |
| :-: | :--- | :--- | :--- | :-: |
| 1 | Inserção e Pesquisa Básica | inserir registo com chave 20230820-SP-123456 e pesquisar a chave. | Registo localizado com sucesso; retorno correto do offset_disco.  | [ x ] |
| 2 | Carga Massiva do Dataset  | Inserção sequencial de N = 48,941 linhas do ficheiro INFLUX 19-23-03-2026.csv.  | Todos os 48,941 nós alocados; árvore mantida autobalançada sem estouro de pilha (Stack Overflow).  | [ x ] |
| 3 | Validação de Ordenação (In-Order)  | Travessia Em-Ordem sobre 10,000 registos inseridos | Exibição das chaves em ordem estritamente alfabética e cronológica crescente.  | [ x ] |
| 4 | Re-balanceamento AVL (Rotações)   | Inserção de chaves forçando casos de LL, RR, LR e RL.  | Rotações simples e duplas acionadas; Fator de Balanceamento FB {-1, 0, 1} em todos os nós. | [ x ] |
| 5 | Divisão de Nó 2-3-4 (Splits)   | Inserção no 4º elemento de um Nó-4 cheio [K1, K2, K3]  | Divisão pré-emptiva (Top-Down Split) executada; chave promovida ao nó pai e folhas no mesmo nível.  | [ x ] |
| 6 | Remoção de Nó Intermediário   | Exclusão de chave correspondente a um nó com 2 filhos. | Substituição da chave pelo sucessor Em-Ordem; re-balanceamento acionado sem perda de subárvores. | [ x ] |
| 7 | Persistência em Ficheiro (Reboot Test)   | Serialização do índice em Pré-Ordem para disco, encerramento do processo e recarga.  | Reconstrução da árvore em RAM mantendo a mesma geometria e altura originais | [ x ] |

### 3.3 Casos Extremos (Edge Cases)

Árvore Vazia: Executar pesquisa ou remoção numa árvore cuja raiz é NULL. O sistema deve retornar NULL ou mensagem informativa sem gerar falha de segmentação (Segmentation Fault).

  Único Elemento: Inserir 1 registo e eliminá-lo em seguida. A raiz deve voltar ao estado NULL e a memória ser libertada corretamente.

  Chaves Duplicadas (Notificações Repetidas): Tentar inserir duas vezes a mesma chave de notificação. O algoritmo deve atualizar o offset existente em vez de criar um nó duplicado.

  Dados em Ordem Cronológica Crescente (Pior Caso de Inserção): Inserção de 48,941 registos ordenados por DT_NOTIFIC.
Resultado na AVL: Rotações contínuas mantêm a altura limitada a 22 níveis.

Resultado na 2-3-4: Divisões preemptivas mantêm todas as folhas estritamente no mesmo nível (altura aproximada de 8 a 9 níveis).

Volume Máximo do Dataset (N = 48,941): Verificação de alocação de memória sob carga real.

Libertação Total de Memória: Destruição completa da árvore via free_tree(). O analisador Valgrind deve reportar 0 bytes lost in 0 blocks.



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
