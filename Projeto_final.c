#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "Projeto_final.h"

ptrFunc foo[5];
/*-----------------------------------------------KERNEL-------------------------------------*/
//Função para controle da escolha do escalonador
void main(){
    foo[0] = sjf_main;
	foo[1] = multiple_priority_queue;
    foo[0]();
    foo[1]();

}
/*-----------------------------------------------SJF----------------------------------------*/
void sjf_main() {//Criado para organizar as chamadas de função do SJF
    Process processes[20];//Definição de número de processos
    int num_processes = 0;

    // Abrir o arquivo stdin.txt
    FILE *file = fopen("stdin.txt", "r");
    if (file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
    }
	//Processamento e leitura dos dados do arquivo
    while (fscanf(file, "%d %d %d", &processes[num_processes].arrival_time, &processes[num_processes].burst_time, &processes[num_processes].priority) == 3) {
        processes[num_processes].id = num_processes + 1;
        num_processes++;
        if (num_processes >= 20) {
            break;
        }
    }
    fclose(file);//Fachar o arquivo

    sjf_scheduler(processes, num_processes);
}


/*--------------------------------------------------------------Priority_Multiple_Queues------------------------------------*/

void multiple_priority_queue() {
    Scheduler s;
    scheduler_init(&s);
    int q1 = scheduler_add_queue(&s);
    int q2 = scheduler_add_queue(&s);

    FILE *input_file = fopen("input.txt", "r");
    if (input_file == NULL) {
        printf("Erro ao abrir o arquivo de entrada.\n");
    }
    Process *pt;
    int priority, time_left;
    int process_count = 0;
    //captura dos dados do arquivo
    while (fscanf(input_file, "%d %d", &priority, &time_left) == 2) {
        //alocando um espaço especifico para cada processo que será criado
        pt = (Process *)malloc(sizeof(Process));
        *pt = create_process(process_count, priority, time_left); // ID atribuído com base no contador de processos

        if (priority > 5) {
            //chamando função de inserção de processos na fila determinada de acordo com a prioridade
            scheduler_enqueue(&s, q1, pt);
            atualizar_maior_prioridade(&(s.queues[q1]));
            trocar_maior_prioridade(&(s.queues[q1]));
        } else {
            //chamando função de inserção de processos na fila determinada
            scheduler_enqueue(&s, q2, pt);
            atualizar_maior_prioridade(&(s.queues[q2]));
            trocar_maior_prioridade(&(s.queues[q2]));
        }

        process_count++;
        printf("\nProcesso %d\n", process_count);
        show_state(&s, q1); // Mostra o estado da fila q1
        show_state(&s, q2); // Mostra o estado da fila q2
    }

    fclose(input_file);
    printf("output start...\n");
    //escrevendo no arquivo de output
    show_state(&s, q1);
    show_state(&s, q2);
    // Resto do código...
}
/*-------------------------------------------------------functions-----------------------------------------------------*/
//incializando o numero de filas
void scheduler_init(Scheduler *s) {
    s->num_queues = 0;
}
//função que adiciona uma fila ao escalonador e retorna seu indice
int scheduler_add_queue(Scheduler *s) {
    if (s->num_queues >= MAX_NUM_QUEUES) {
        printf("Número máximo de filas atingido.\n");
        return -1;
    }
    Queue *q = &(s->queues[s->num_queues]);
    q->head = 0;
    q->tail = 0;
    q->maior = 0;
    q->num_process = 0;
    s->num_queues++;
    return s->num_queues - 1;
}
//função que seleciona o processo de maior prioridade e manda para a posição head para ser processado
void trocar_maior_prioridade(Queue *q) {
    int indice_maior = q->maior;
    Process *processo_maior = q->buffer[indice_maior];
    Process *processo_head = q->buffer[q->head];

    q->buffer[indice_maior] = processo_head;
    q->buffer[q->head] = processo_maior;
    q->maior = q->head;
}
//atualiza o processo de maior prioridade
void atualizar_maior_prioridade(Queue *q) {
    int maior_prioridade = -1;
    int indice_maior = -1;

    for (int i = q->head; i != q->tail; i = (i + 1) % MAX_QUEUE_SIZE) {
        Process *processo = q->buffer[i];
        if (processo != NULL && processo->priority > maior_prioridade) {
            maior_prioridade = processo->priority;
            indice_maior = i;
        }
    }

    q->maior = indice_maior;
}
//adiciona um processo ao buffer, e se caso estiver cheio desloca o head consumindo o processo "executado"
//abrindo espaço para uma inserção
int enqueue(Queue *q, Process *p) {
    if ((q->tail + 1) % MAX_QUEUE_SIZE == q->head) {
        Process *removed_process = dequeue(q); // Remove o processo head
        free(removed_process);
    } else if (q->num_process > 0 && q->tail == q->head) {
        q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    }

    q->num_process++;
    p->id = q->num_process; // Atualiza o id do processo
    q->buffer[q->tail] = p;
    q->tail = (q->tail + 1) % MAX_QUEUE_SIZE;
    return 0;
}
//remove um processo da cabeça do buffer
Process *dequeue(Queue *q) {
    if (q->head == q->tail) {
        return NULL;
    }

    Process *p = q->buffer[q->head];
    q->buffer[q->head] = NULL;
    q->head = (q->head + 1) % MAX_QUEUE_SIZE;
    return p;
}
//insere um processo em uma fila determinada
int scheduler_enqueue(Scheduler *s, int queue_idx, Process *p) {
    if (queue_idx < 0 || queue_idx >= s->num_queues) {
        return -1; // índice de fila inválido
    }
    return enqueue(&(s->queues[queue_idx]), p);
}
//remove um processo de uma fila determinada
void *scheduler_dequeue(Scheduler *s, int idx) {
    Queue *q = &(s->queues[idx]);
    Process *p = dequeue(q);
    if (p != NULL) {
        return p;
    }
    return NULL; // todas as filas estão vazias
}
//cria um processo com parâmetros retirados do arquivo
Process create_process(int id, int priority, int timeLeft) {
    Process p;
    p.id = id; // Atribui o ID fornecido
    p.priority = priority;
    p.time_left = timeLeft;
    p.running = 0;
    return p;
}
// exibe/escreve no arquivo de saída somente os processos dentro do escopo atual
void show_state(Scheduler *s, int idx) {
    Queue *q = &(s->queues[idx]);
    int i = q->head;
    int count = 0;
    FILE *output_file = fopen("output.txt", "a"); // Abre o arquivo em modo de append ("a")
    if (output_file == NULL) {
        printf("Erro ao abrir o arquivo de saída.\n");
        return;
    }
    fprintf(output_file, "\nEstado da fila %d:\n", idx);
    fprintf(output_file, "Maior %d, Head %d, Tail %d\n", q->maior, q->head, q->tail);
    
    while (i != q->tail) {
        Process *processo = q->buffer[i];
        if (processo != NULL) {
            fprintf(output_file, "Processo ID: %d, Prioridade: %d\n", processo->id, processo->priority);
            count++;
        }
        i = (i + 1) % MAX_QUEUE_SIZE;
    }
    
    fclose(output_file);
}

void sjf_scheduler(Process processes[], int num_processes) { //CPU Schedule SJF
    int current_time = 0;
    int completed_processes = 0;
	//Criar o arquivo de escrita
    FILE *file = fopen("stdout.txt", "w");
    if (file == NULL) {
        printf("Erro ao criar o arquivo de saída.\n");
        return;
    }

    while (completed_processes < num_processes) {
        int shortest_job_index = -1; // Índice do processo com o menor tempo de execução
        int shortest_burst_time = -1; // Tempo de execução do processo mais curto

        // Procurar o próximo processo a ser executado
        for (int i = 0; i < num_processes; i++) {
            // Verificar se o processo chegou (arrival_time <= current_time)
            // e se ainda tem tempo de execução restante (burst_time > 0)
            if (processes[i].arrival_time <= current_time && processes[i].burst_time > 0) {
                // Verificar se é o processo com o menor tempo de execução até agora
                if (shortest_job_index == -1 || processes[i].burst_time < shortest_burst_time) {
                    shortest_job_index = i;
                    shortest_burst_time = processes[i].burst_time;
                }
            }
        }

        // Se não houver processos elegíveis para execução no momento,
        // incrementar o tempo atual e continuar para a próxima iteração
        if (shortest_job_index == -1) {
            current_time++;
            continue;
        }

        Process *shortest_job = &processes[shortest_job_index]; // Processo com o menor tempo de execução
        fprintf(file, "Executando processo ID: %d Chegada: %d Execução: %d\n", shortest_job->id, shortest_job->arrival_time, shortest_job->burst_time);
        shortest_job->burst_time--; // Decrementar o tempo de execução do processo
        current_time++; // Incrementar o tempo atual

        // Verificar se o tempo de execução do processo chegou a zero,
        // indicando que o processo foi concluído
        if (shortest_job->burst_time == 0) {
            completed_processes++; // Incrementar o contador de processos concluídos
        }
    }

    fprintf(file, "Todos os processos foram executados.\n"); // Mensagem indicando a conclusão de todos os processos
    fclose(file); // Fechar o arquivo
}