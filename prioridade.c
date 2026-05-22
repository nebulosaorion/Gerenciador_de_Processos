#include <stdio.h>
#include <stdlib.h>

// Estrutura externa definida em processos.c
typedef struct {
    int momento_criacao;
    int pid;
    int tempo_execucao;
    int prioridade_bilhetes;
} Processo;

// Estrutura interna para facilitar o controle na Prioridade
typedef struct {
    int pid;
    int momento_criacao;
    int tempo_execucao;
    int tempo_restante;
    int prioridade;
    int na_fila;      
    int concluido;    
    int ja_executou;  
    int latencia;
    int tempo_conclusao;
    int ultimo_uso;   // NOVO: Controla o Round-Robin em caso de empate
} ProcPrioridade;

// --- Lógica do Escalonador por Prioridade ---

void escalonamento_prioridade(Processo *processos, int num_processos, int quantum, const char *arquivo_saida) {
    ProcPrioridade *procs = (ProcPrioridade*)malloc(num_processos * sizeof(ProcPrioridade));
    for (int i = 0; i < num_processos; i++) {
        procs[i].pid = processos[i].pid;
        procs[i].momento_criacao = processos[i].momento_criacao;
        procs[i].tempo_execucao = processos[i].tempo_execucao;
        procs[i].tempo_restante = processos[i].tempo_execucao;
        procs[i].prioridade = processos[i].prioridade_bilhetes;
        procs[i].na_fila = 0;
        procs[i].concluido = 0;
        procs[i].ja_executou = 0;
        procs[i].latencia = 0;
        procs[i].ultimo_uso = processos[i].momento_criacao; // Inicializa com o tempo que nasceu
    }

    FILE *out = fopen(arquivo_saida, "w");
    
    int tempo_atual = 0;
    int concluidos = 0;

    printf("Iniciando Escalonamento por Prioridade...\n");

    while (concluidos < num_processos) {
        // 1. Verifica quem chegou no sistema
        for (int i = 0; i < num_processos; i++) {
            if (!procs[i].na_fila && procs[i].momento_criacao <= tempo_atual) {
                procs[i].na_fila = 1;
            }
        }

        // 2. Encontra o processo de MAIOR prioridade disponível (Menor Número = Maior Prioridade)
        int idx_escolhido = -1;
        int melhor_prioridade = 999999; // Começa com um valor absurdo para ir diminuindo
        int tempo_mais_antigo = 999999;

        for (int i = 0; i < num_processos; i++) {
            if (procs[i].na_fila && !procs[i].concluido) {
                
                // Se encontrou uma prioridade numéricamente MENOR (mais urgente)
                if (procs[i].prioridade < melhor_prioridade) {
                    melhor_prioridade = procs[i].prioridade;
                    idx_escolhido = i;
                    tempo_mais_antigo = procs[i].ultimo_uso;
                } 
                // REGRA DE DESEMPATE: Mesma prioridade? Roda quem encostou na CPU há mais tempo (Round-Robin)
                else if (procs[i].prioridade == melhor_prioridade) {
                    if (procs[i].ultimo_uso < tempo_mais_antigo) {
                        idx_escolhido = i;
                        tempo_mais_antigo = procs[i].ultimo_uso;
                    }
                }
            }
        }

        if (idx_escolhido != -1) {
            ProcPrioridade *p = &procs[idx_escolhido];

            if (p->ja_executou == 0) {
                p->latencia = tempo_atual - p->momento_criacao;
                p->ja_executou = 1;
                printf(">>> ALERTA: PID %d entrou na CPU pela PRIMEIRA VEZ (Latencia: %dms) <<<\n", p->pid, p->latencia);
            }

            int tempo_rodar = (p->tempo_restante < quantum) ? p->tempo_restante : quantum;

            printf("Tempo %d: PID %d na CPU (Prioridade %d | Restava %dms | Executando %dms)\n",
                   tempo_atual, p->pid, p->prioridade, p->tempo_restante, tempo_rodar);

            p->tempo_restante -= tempo_rodar;
            tempo_atual += tempo_rodar;
            
            // Atualiza o relógio desse processo para ele ir para o final da fila de desempate
            p->ultimo_uso = tempo_atual; 

            if (p->tempo_restante <= 0) {
                p->concluido = 1;
                p->tempo_conclusao = tempo_atual;
                concluidos++;
            }
        } else {
            tempo_atual++;
        }
    }

    // ==========================================================
    // IMPRESSÃO DUPLA (ARQUIVO E TERMINAL)
    // ==========================================================
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
    free(procs);
    printf("\nEscalonamento por Prioridade concluido com sucesso! Leia o arquivo %s\n", arquivo_saida);
}