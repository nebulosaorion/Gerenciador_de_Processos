#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
   Funcionamento:
   - Cada processo recebe um quantum de tempo na CPU
   - Se não terminar no quantum, vai para o fim da fila
   - Repete até todos os processos terminarem
   ============================================================ */

/* ---------- Estrutura de um processo ---------- */
typedef struct {
    int pid;               /* Identificador do processo          */
    int tempo_de_chegada;     /* Instante em que chega na fila      */
    int tempo_de_execucao;    /* Tempo total que precisa de CPU      */
    int tempo_de_restante;    /* Tempo que ainda falta executar      */
    int tempo_de_espera;      /* Tempo total esperando na fila      */
    int tempo_de_conclusao;   /* Instante em que terminou           */
    int turnaround;        /* tempo_conclusao - tempo_chegada    */
} Processo;

/* ---------- Fila circular (implementação simples com array) ---------- */
typedef struct {
    int *dados;       /* Array com os índices dos processos na fila */
    int inicio;       /* Posição do primeiro elemento               */
    int fim;          /* Posição onde o próximo será inserido        */
    int tamanho;      /* Quantos processos estão na fila agora       */
    int capacidade;   /* Tamanho máximo da fila                      */
} FilaCircular;

/* Cria a fila com capacidade máxima */
FilaCircular* criar_fila(int capacidade) {
    FilaCircular *fila = malloc(sizeof(FilaCircular));
    fila->dados     = malloc(capacidade * sizeof(int));
    fila->inicio    = 0;
    fila->fim       = 0;
    fila->tamanho   = 0;
    fila->capacidade = capacidade;
    return fila;
}

/* Verifica se a fila está vazia */
int fila_vazia(FilaCircular *fila) {
    return fila->tamanho == 0;
}

/* Insere o índice de um processo no fim da fila */
void enfileirar(FilaCircular *fila, int indice_processo) {
    fila->dados[fila->fim] = indice_processo;
    fila->fim = (fila->fim + 1) % fila->capacidade; /* avança circularmente */
    fila->tamanho++;
}

/* Remove e retorna o índice do processo no início da fila */
int desenfileirar(FilaCircular *fila) {
    int indice = fila->dados[fila->inicio];
    fila->inicio = (fila->inicio + 1) % fila->capacidade; /* avança circularmente */
    fila->tamanho--;
    return indice;
}

/* Libera a memória da fila */
void destruir_fila(FilaCircular *fila) {
    free(fila->dados);
    free(fila);
}

/* ---------- Função principal do Round-Robin ---------- */
void executar_round_robin(Processo *processos, int quantidade, int quantum) {

    int tempo_atual = 0;          /* Relógio da simulação              */
    int processos_concluidos = 0; /* Contador de processos que acabaram */

    /* Fila de processos prontos para executar */
    FilaCircular *fila_prontos = criar_fila(quantidade);

    /* Marca quais processos já entraram na fila (para não inserir duas vezes) */
    int *ja_enfileirado = calloc(quantidade, sizeof(int));

    printf("Simulacao Round-Robin (quantum = %d)\n\n", quantum);
    printf("%-6s %-10s\n", "Tempo", "Evento");
    printf("--------------------------------------\n");

    /* Coloca na fila os processos que chegam no tempo 0 */
    for (int i = 0; i < quantidade; i++) {
        if (processos[i].tempo_de_chegada == 0) {
            enfileirar(fila_prontos, i);
            ja_enfileirado[i] = 1;
        }
    }

    /* Loop principal: roda enquanto houver processos não concluídos */
    while (processos_concluidos < quantidade) {

        /* Se a fila está vazia mas ainda existem processos,
           avança o tempo até o próximo processo chegar */
        if (fila_vazia(fila_prontos)) {
            tempo_atual++;

            /* Verifica se algum processo chegou neste instante */
            for (int i = 0; i < quantidade; i++) {
                if (!ja_enfileirado[i] && processos[i].tempo_de_chegada <= tempo_atual) {
                    enfileirar(fila_prontos, i);
                    ja_enfileirado[i] = 1;
                }
            }
            continue;
        }

        /* Pega o próximo processo da fila */
        int idx = desenfileirar(fila_prontos);

        /* Calcula quanto tempo este processo vai usar agora */
        int tempo_usado;
        if (processos[idx].tempo_de_restante <= quantum) {
            /* Processo termina antes do quantum acabar */
            tempo_usado = processos[idx].tempo_de_restante;
        } else {
            /* Processo usa o quantum completo e volta para a fila */
            tempo_usado = quantum;
        }

        printf("t=%-4d P%d executa por %d unidade(s)\n",
               tempo_atual, processos[idx].pid, tempo_usado);

        /* Atualiza o tempo de espera de todos os outros processos
           que estão na fila prontos enquanto este está executando */
        for (int i = 0; i < fila_prontos->tamanho; i++) {
            int outro = fila_prontos->dados[(fila_prontos->inicio + i) % fila_prontos->capacidade];
            processos[outro].tempo_de_espera += tempo_usado;
        }

        /* Avança o relógio */
        tempo_atual += tempo_usado;
        processos[idx].tempo_de_restante -= tempo_usado;

        /* Verifica se novos processos chegaram durante este intervalo */
        for (int i = 0; i < quantidade; i++) {
            if (!ja_enfileirado[i] && processos[i].tempo_de_chegada <= tempo_atual) {
                enfileirar(fila_prontos, i);
                ja_enfileirado[i] = 1;
                processos[i].tempo_de_espera += tempo_atual - processos[i].tempo_de_chegada;
            }
        }

        /* Verifica se o processo terminou */
        if (processos[idx].tempo_de_restante == 0) {
            processos[idx].tempo_de_conclusao = tempo_atual;
            processos[idx].turnaround      = tempo_atual - processos[idx].tempo_de_chegada;
            processos_concluidos++;
            printf("t=%-4d P%d CONCLUIDO\n", tempo_atual, processos[idx].pid);
        } else {
            /* Processo não terminou: volta para o fim da fila */
            enfileirar(fila_prontos, idx);
        }
    }

    /* Libera memória */
    destruir_fila(fila_prontos);
    free(ja_enfileirado);
}

/* ---------- Gera o arquivo de resultado ---------- */
void gerar_relatorio(Processo *processos, int quantidade, int quantum) {
    FILE *arquivo = fopen("resultadoRoundRobin.txt", "w");
    if (arquivo == NULL) {
        printf("Erro ao criar arquivo de resultado!\n");
        return;
    }

    fprintf(arquivo, "RESULTADO: Round-Robin (quantum = %d)\n\n", quantum);
    fprintf(arquivo, "%-6s %-10s %-10s %-12s %-12s %-12s\n",
            "PID", "Chegada", "Burst", "Espera", "Conclusao", "Turnaround");
    fprintf(arquivo, "--------------------------------------------------------------\n");

    double soma_espera     = 0;
    double soma_turnaround = 0;

    for (int i = 0; i < quantidade; i++) {
        fprintf(arquivo, "%-6d %-10d %-10d %-12d %-12d %-12d\n",
                processos[i].pid,
                processos[i].tempo_de_chegada,
                processos[i].tempo_de_execucao,
                processos[i].tempo_de_espera,
                processos[i].tempo_de_conclusao,
                processos[i].turnaround);

        soma_espera     += processos[i].tempo_de_espera;
        soma_turnaround += processos[i].turnaround;
    }

    fprintf(arquivo, "\n--- Medias ---\n");
    fprintf(arquivo, "Tempo medio de espera    : %.2f\n", soma_espera / quantidade);
    fprintf(arquivo, "Turnaround medio         : %.2f\n", soma_turnaround / quantidade);

    fclose(arquivo);
    printf("\nResultado salvo em: resultadoRoundRobin.txt\n");
}

/* ---------- Lê processos de um arquivo ---------- */
int ler_arquivo(Processo **processos, const char *nome_arquivo) {
    FILE *arquivo = fopen(nome_arquivo, "r");
    if (arquivo == NULL) {
        printf("Erro: arquivo '%s' nao encontrado.\n", nome_arquivo);
        return 0;
    }

    int quantidade = 0;
    fscanf(arquivo, "%d", &quantidade); /* primeira linha: número de processos */

    *processos = malloc(quantidade * sizeof(Processo));

    int prioridade;
    for (int i = 0; i < quantidade; i++) {
        fscanf(arquivo, "%d %d %d %d",
               &(*processos)[i].pid,
               &(*processos)[i].tempo_de_chegada,
               &(*processos)[i].tempo_de_execucao,
               &prioridade); /* lê mas Round-Robin ignora prioridade */

        /* Inicializa campos calculados */
        (*processos)[i].tempo_de_restante  = (*processos)[i].tempo_de_execucao;
        (*processos)[i].tempo_de_espera    = 0;
        (*processos)[i].tempo_de_conclusao = 0;
        (*processos)[i].turnaround      = 0;
    }

    fclose(arquivo);
    return quantidade;
}

/* ---------- Gera processos aleatoriamente ---------- */
int gerar_dinamico(Processo **processos) {
    int quantidade;
    printf("Quantos processos deseja gerar? ");
    scanf("%d", &quantidade);

    *processos = malloc(quantidade * sizeof(Processo));

    printf("\nProcessos gerados:\n");
    printf("%-6s %-10s %-10s\n", "PID", "Chegada", "Burst");

    for (int i = 0; i < quantidade; i++) {
        (*processos)[i].pid            = i + 1;
        (*processos)[i].tempo_de_chegada  = rand() % 10;        /* chegada entre 0 e 9  */
        (*processos)[i].tempo_de_execucao = (rand() % 10) + 1;  /* burst entre 1 e 10   */
        (*processos)[i].tempo_de_restante = (*processos)[i].tempo_de_execucao;
        (*processos)[i].tempo_de_espera   = 0;
        (*processos)[i].tempo_de_conclusao = 0;
        (*processos)[i].turnaround     = 0;

        printf("%-6d %-10d %-10d\n",
               (*processos)[i].pid,
               (*processos)[i].tempo_de_chegada,
               (*processos)[i].tempo_de_execucao);
    }

    return quantidade;
}

/* ---------- Menu principal ---------- */
int main() {
    srand(42); /* Semente fixa para resultados reproduzíveis nos testes */

    Processo *processos = NULL;
    int quantidade      = 0;
    int quantum         = 0;
    int opcao;

    printf("Escalonador Round-Robin\n\n");
    printf("1 - Ler processos do arquivo (entradaEscalonador.txt)\n");
    printf("2 - Gerar processos dinamicamente\n");
    printf("Opcao: ");
    scanf("%d", &opcao);

    if (opcao == 1) {
        quantidade = ler_arquivo(&processos, "entradaEscalonador.txt");
        if (quantidade == 0) return 1; /* erro na leitura */
    } else {
        quantidade = gerar_dinamico(&processos);
    }

    printf("\nInforme o quantum: ");
    scanf("%d", &quantum);

    /* Executa o algoritmo */
    executar_round_robin(processos, quantidade, quantum);

    /* Gera o arquivo de resultado */
    gerar_relatorio(processos, quantidade, quantum);

    free(processos);
    return 0;
}