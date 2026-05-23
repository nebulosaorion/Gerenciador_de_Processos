#include <stdio.h>
#include <stdlib.h>
<<<<<<< Updated upstream

// Estrutura externa definida em processos.c
typedef struct {
    int momento_criacao;
    int pid;
    int tempo_execucao;
    int prioridade_bilhetes; // Não é usada no RR, mas mantemos pelo padrão do processos.c
} Processo;

=======
#include "processos.h"

>>>>>>> Stashed changes
// Estrutura interna para facilitar o controle na Alternância Circular
typedef struct {
    int pid;
    int momento_criacao;
    int tempo_execucao;
    int tempo_restante;
    int na_fila;      
    int concluido;    
    int ja_executou;  
    int latencia;
    int tempo_conclusao;
} ProcRR;

// --- Lógica do Escalonador Alternância Circular (Round-Robin) ---

void alternancia_circular(Processo *processos, int num_processos, int quantum, const char *arquivo_saida) {
<<<<<<< Updated upstream
    ProcRR *procs = (ProcRR*)malloc(num_processos * sizeof(ProcRR));
=======
    if (processos == NULL) {
        printf("Erro: lista de processos invalida.\n");
        return;
    }

    if (num_processos <= 0) {
        printf("Erro: numero de processos invalido.\n");
        return;
    }

    if (quantum <= 0) {
        printf("Erro: quantum invalido.\n");
        return;
    }

    if (arquivo_saida == NULL) {
        printf("Erro: arquivo de saida invalido.\n");
        return;
    }

    ProcRR *procs = (ProcRR*)malloc(num_processos * sizeof(ProcRR));
    if (procs == NULL) {
        printf("Erro de memoria ao alocar processos do Round-Robin.\n");
        return;
    }

>>>>>>> Stashed changes
    for (int i = 0; i < num_processos; i++) {
        procs[i].pid = processos[i].pid;
        procs[i].momento_criacao = processos[i].momento_criacao;
        procs[i].tempo_execucao = processos[i].tempo_execucao;
        procs[i].tempo_restante = processos[i].tempo_execucao;
        procs[i].na_fila = 0;
        procs[i].concluido = 0;
        procs[i].ja_executou = 0;
        procs[i].latencia = 0;
<<<<<<< Updated upstream
=======
        procs[i].tempo_conclusao = 0;
    }

    FILE *out = fopen(arquivo_saida, "w");
    if (out == NULL) {
        printf("Erro ao abrir o arquivo de saida: %s\n", arquivo_saida);
        free(procs);
        return;
>>>>>>> Stashed changes
    }
    
    // fila circular simples  
    int *fila = (int*)malloc(num_processos * sizeof(int));
    if (fila == NULL) {
        printf("Erro de memoria ao alocar fila do Round-Robin.\n");
        fclose(out);
        free(procs);
        return;
    }
    int inicio_fila = 0;
    int tamanho_fila = 0;

<<<<<<< Updated upstream
    FILE *out = fopen(arquivo_saida, "w");
    
    // --- FILA CIRCULAR SIMPLES PARA GERENCIAR A ORDEM ---
    int *fila = (int*)malloc(num_processos * sizeof(int));
    int inicio_fila = 0;
    int tamanho_fila = 0;

    int tempo_atual = 0;
    int concluidos = 0;

    printf("Iniciando Escalonamento por Alternancia Circular (Round-Robin)...\n");

    while (concluidos < num_processos) {
        // 1. Verifica quem chegou no instante atual (tempo_atual) e coloca na fila
        for (int i = 0; i < num_processos; i++) {
            if (!procs[i].na_fila && procs[i].momento_criacao <= tempo_atual && !procs[i].concluido) {
                fila[(inicio_fila + tamanho_fila) % num_processos] = i; // Enfileira
                tamanho_fila++;
                procs[i].na_fila = 1;
            }
        }

        // 2. Processa quem está na frente da fila
        if (tamanho_fila > 0) {
            // Desenfileira o primeiro da fila
            int idx_escolhido = fila[inicio_fila];
            inicio_fila = (inicio_fila + 1) % num_processos;
            tamanho_fila--;
            
            ProcRR *p = &procs[idx_escolhido];

            // Print da Fila de Prontos
            printf("\nFila de Prontos aguardando: ");
            if (tamanho_fila == 0) {
                printf("Vazia");
            } else {
                for(int k = 0; k < tamanho_fila; k++) {
                    printf("[PID %d] ", procs[fila[(inicio_fila + k) % num_processos]].pid);
                }
            }
            printf("\n");

            // ---> CÁLCULO DA LATÊNCIA <---
            if (p->ja_executou == 0) {
                p->latencia = tempo_atual - p->momento_criacao;
                p->ja_executou = 1;
                printf(">>> ALERTA: PID %d entrou na CPU pela PRIMEIRA VEZ (Latencia: %dms) <<<\n", p->pid, p->latencia);
            }

            int tempo_rodar = (p->tempo_restante < quantum) ? p->tempo_restante : quantum;

            printf("Tempo %d: PID %d na CPU (Restava %dms | Executando %dms)\n",
                   tempo_atual, p->pid, p->tempo_restante, tempo_rodar);

            int tempo_final_fatia = tempo_atual + tempo_rodar;

            // 3. A MÁGICA DO ROUND-ROBIN: Processos podem chegar enquanto este roda!
            // Precisamos enfileirar os novatos ANTES de devolver o processo atual para a fila.
=======
    int tempo_atual = 0;
    int concluidos = 0;

    printf("Iniciando Escalonamento por Alternancia Circular \n");

    while (concluidos < num_processos) {
        // Verifica quem chegou no instante atual e coloca na fila
        for (int i = 0; i < num_processos; i++) {
            if (!procs[i].na_fila && procs[i].momento_criacao <= tempo_atual && !procs[i].concluido) {
                fila[(inicio_fila + tamanho_fila) % num_processos] = i; // Enfileira
                tamanho_fila++;
                procs[i].na_fila = 1;
            }
        }

        // Processa quem está na frente da fila
        if (tamanho_fila > 0) {
            // Desenfileira o primeiro da fila
            int idx_escolhido = fila[inicio_fila];
            inicio_fila = (inicio_fila + 1) % num_processos;
            tamanho_fila--;
            
            ProcRR *p = &procs[idx_escolhido];

            // Print da Fila de Prontos
            printf("\nFila de Prontos aguardando: ");
            if (tamanho_fila == 0) {
                printf("Vazia");
            } else {
                for(int k = 0; k < tamanho_fila; k++) {
                    printf("[PID %d] ", procs[fila[(inicio_fila + k) % num_processos]].pid);
                }
            }
            printf("\n");

            // calculo de latencia
            if (p->ja_executou == 0) {
                p->latencia = tempo_atual - p->momento_criacao;
                p->ja_executou = 1;
                printf("ALERTA: PID %d entrou na CPU pela PRIMEIRA VEZ (Latencia: %dms) <<<\n", p->pid, p->latencia);
            }

            int tempo_rodar = (p->tempo_restante < quantum) ? p->tempo_restante : quantum;

            printf("Tempo %d: PID %d na CPU (Restava %dms | Executando %dms)\n",
                   tempo_atual, p->pid, p->tempo_restante, tempo_rodar);

            int tempo_final_fatia = tempo_atual + tempo_rodar;

            //  Processos podem chegar enquanto este roda
            // Precisamos enfileirar os novatos antes de devolver o processo atual para a fila.
>>>>>>> Stashed changes
            for (int t = tempo_atual + 1; t <= tempo_final_fatia; t++) {
                for (int i = 0; i < num_processos; i++) {
                    if (!procs[i].na_fila && procs[i].momento_criacao == t) {
                        fila[(inicio_fila + tamanho_fila) % num_processos] = i; // Enfileira o novato
                        tamanho_fila++;
                        procs[i].na_fila = 1;
                    }
                }
            }

            p->tempo_restante -= tempo_rodar;
            tempo_atual = tempo_final_fatia; // Avança o relógio

<<<<<<< Updated upstream
            // 4. Verifica o destino do processo atual
=======
            //Verifica o destino do processo atual
>>>>>>> Stashed changes
            if (p->tempo_restante <= 0) {
                p->concluido = 1;
                p->tempo_conclusao = tempo_atual;
                concluidos++;
            } else {
<<<<<<< Updated upstream
                // Se não terminou, volta pro FINAL da fila
=======
                // Se não terminou, volta pro final da fila
>>>>>>> Stashed changes
                fila[(inicio_fila + tamanho_fila) % num_processos] = idx_escolhido;
                tamanho_fila++;
            }
        } else {
            // Se a fila tá vazia, só avança o relógio
            tempo_atual++;
        }
    }

<<<<<<< Updated upstream
    // ==========================================================
    // IMPRESSÃO DUPLA (ARQUIVO E TERMINAL)
    // ==========================================================
=======
    
    // IMPRESSÃO DUPLA (ARQUIVO E TERMINAL)
>>>>>>> Stashed changes
    fprintf(out, "PID | Latencia | Tpronto | Tempo de Turnaround\n");
    
    printf("\n========================================================\n");
    printf("             RELATORIO FINAL DE DESEMPENHO              \n");
    printf("========================================================\n");
    printf("PID | Latencia | Tpronto | Tempo de Turnaround\n");

    for (int i = 0; i < num_processos; i++) {
        int turnaround = procs[i].tempo_conclusao - procs[i].momento_criacao;
        int tpronto = turnaround - procs[i].tempo_execucao;
        
        // Escreve no arquivo de texto
        fprintf(out, "%d | %d | %d | %d\n", procs[i].pid, procs[i].latencia, tpronto, turnaround);
        
        // Imprime direto no terminal
        printf("%d   | %d        | %d       | %d\n", procs[i].pid, procs[i].latencia, tpronto, turnaround);
    }
    printf("========================================================\n");

    fclose(out);
    free(fila);
    free(procs);
    printf("\nEscalonamento Alternancia Circular concluido com sucesso! Leia o arquivo %s\n", arquivo_saida);
<<<<<<< Updated upstream
}
=======
}
>>>>>>> Stashed changes
