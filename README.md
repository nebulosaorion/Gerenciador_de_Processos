# Escalonador de Processos

Implementação de um escalonador preemptivo de processos em C, desenvolvida como trabalho da disciplina de Sistemas Operacionais (FURG).

O programa simula o gerenciamento de processos na CPU utilizando quatro algoritmos de escalonamento diferentes, com menu para leitura de arquivo ou geração dinâmica de processos.

---

## Algoritmos implementados

| Arquivo | Algoritmo |
|---|---|
| `alternancia_circular.c` | Round-Robin — fatias de tempo iguais (quantum) |
| `prioridade.c` | Prioridade — processo com maior prioridade executa primeiro |
| `loteria.c` | Loteria — sorteio proporcional ao número de bilhetes |
| `cfs.c` | CFS (Completely Fair Scheduler) — baseado em Árvore Rubro-Negra |
| `processos.c` | Arquivo principal — menu e integração dos algoritmos |

---

## Como compilar

```bash
gcc processos.c alternancia_circular.c prioridade.c loteria.c cfs.c -o escalonador
```

---

## Como executar

```bash
./escalonador
```

O programa apresenta um menu com duas opções:

- **Opção 1** — lê os processos do arquivo `entradaEscalonador.txt`
- **Opção 2** — gera processos aleatoriamente

---

## Formato do arquivo de entrada

```
algoritmoDeEscalonamento|fraçãoDeCPU
momentoDeCriação|PID|tempoDeExecução|prioridade
momentoDeCriação|PID|tempoDeExecução|prioridade
...
```

Onde:
- `algoritmoDeEscalonamento` — `alternanciaCircular`, `prioridade`, `loteria` ou `CFS`
- `fraçãoDeCPU` — tempo máximo que um processo pode ficar na CPU por vez (quantum)
- `momentoDeCriação` — instante em que o processo chega na fila
- `PID` — identificador único do processo
- `tempoDeExecução` — tempo total que o processo precisa de CPU
- `prioridade` — prioridade do processo (ou número de bilhetes no algoritmo da loteria)

Exemplo:

```
alternanciaCircular|4
0|1|36|13
1|2|32|10
2|3|20|28
3|4|12|68
```

Para gerar o arquivo automaticamente use o script fornecido:

```bash
python3 geradorEntrada.py
```

---

## Saída

Durante a simulação o programa mostra qual processo está na CPU e quanto tempo falta para terminar:

```
Simulacao Round-Robin (quantum = 4)

Tempo  Evento
--------------------------------------
t=0    P1 executa por 4 unidade(s)
t=4    P2 executa por 4 unidade(s)
...
t=96   P1 CONCLUIDO
```

Ao final é gerado um arquivo `.txt` com os resultados de cada processo:

- Tempo de chegada e burst
- Tempo de espera (estado "pronto")
- Tempo de conclusão
- Turnaround (conclusão − chegada)
- Média de espera e turnaround do conjunto

---

## Estrutura do projeto

```
.
├── processos.c              # menu principal e integração
├── alternancia_circular.c   # Round-Robin
├── prioridade.c             # Escalonamento por prioridade
├── loteria.c                # Escalonamento por loteria
├── cfs.c                    # CFS com Árvore Rubro-Negra
└── geradorEntrada.py        # gerador de arquivos de entrada
```
