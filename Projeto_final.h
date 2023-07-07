#ifndef PROJETO_FINAL_H 
#define PROJETO_FINAL_H 
#define MAX_QUEUE_SIZE 5
#define MAX_NUM_QUEUES 10
//declaração da estrutura de processo
typedef struct {
    int id;
    int priority;
    int arrival_time;
    int burst_time;
    int time_left;
    int running;
} Process;
//declaração da estrutura de fila em formato de buffer circular
typedef struct {
    Process *buffer[MAX_QUEUE_SIZE];
    int head;
    int tail;
    int maior;
    int num_process;
} Queue;
//declaração do escalonador que mantém 
typedef struct {
    Queue queues[MAX_NUM_QUEUES];
    int num_queues;
} Scheduler;
//ponteiro que receberá a função determinada
typedef void(*ptrFunc)(void);

void scheduler_init(Scheduler *s);
int scheduler_add_queue(Scheduler *s);
void trocar_maior_prioridade(Queue *q);
void atualizar_maior_prioridade(Queue *q);
int enqueue(Queue *q, Process *p);
Process *dequeue(Queue *q);
int scheduler_enqueue(Scheduler *s, int queue_idx, Process *p);
void *scheduler_dequeue(Scheduler *s, int idx);
Process create_process(int id, int priority, int timeLeft);
void show_state(Scheduler *s, int idx);
void sjf_scheduler(Process processes[], int num_processes);
void sjf_main();
void multiple_priority_queue();

#endif 