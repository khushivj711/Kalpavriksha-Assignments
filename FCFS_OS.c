#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define HASH_MAP_SIZE 1009
#define MAX_NAME_LEN 64
#define MAX_PROCS 1000
#define MAX_EVENTS 1000

typedef enum
{
    READY,
    RUNNING,
    WAITING,
    TERMINATED
} State;

typedef struct PCB
{
    char name[MAX_NAME_LEN];
    int pid;

    int cpu_burst;
    int cpu_remaining;

    int io_start;
    int io_duration;
    int io_remaining;

    int executed_time;
    int completion_time;
    int io_time_total;

    int killed;
    int killed_time;

    State state;
    struct PCB *next;
} PCB;

typedef struct Queue
{
    PCB *front;
    PCB *rear;
} Queue;

typedef struct HashMapNode
{
    int key;
    PCB *process;
    struct HashMapNode *next;
} HashMapNode;

typedef struct KillEvent
{
    int pid;
    int time;
} KillEvent;

int hashFunction(int key);
void hashPut(int key, PCB *process);
PCB *hashGet(int key);

void initQueue(Queue *q);
void enqueue(Queue *q, PCB *p);
PCB *dequeue(Queue *q);
PCB *removeFromQueue(Queue *q, int pid);
void forEachQueue(Queue *q, void (*fn)(PCB *, void *), void *ctx);

PCB *createPCB(const char *name, int pid, int burst, int io_start, int io_dur);
void trim(char *s); 
void parseLine(char *line);
void decIO(PCB *p, void *ctx);
void checkIOCompletion();
void terminatePCB(PCB *p, int time);
void applyKillEvents(int time, PCB **running, int *terminated);
void simulate();
int cmpPID(const void *a, const void *b);
void printResults();

HashMapNode *hashMap[HASH_MAP_SIZE];
Queue readyQ, waitingQ, terminatedQ;
PCB *procList[MAX_PROCS];
KillEvent killList[MAX_EVENTS];

int procCount = 0;
int killCount = 0;
int totalProcesses = 0;

int main()
{
    initQueue(&readyQ);
    initQueue(&waitingQ);
    initQueue(&terminatedQ);

    char line[256];

    while (fgets(line, sizeof(line), stdin))
    {
        parseLine(line);
    }

    if (totalProcesses == 0)
    {
        printf("No processes entered.\n");
        return 0;
    }

    simulate();
    printResults();
    return 0;
}

void trim(char *s)
{
    int i = 0, j = 0;

    while (isspace(s[i]))
    {
        i++;
    }

    while (s[i])
    {
        s[j++] = s[i++];
    }

    s[j] = '\0';

    while (j > 0 && isspace(s[j - 1]))
    {
        s[--j] = '\0';
    }
}

void parseLine(char *line)
{
    trim(line);
    if (strlen(line) == 0)
    {
        return;
    }

    char first[16];
    sscanf(line, "%s", first);

    if (strcasecmp(first, "KILL") == 0) 
    {
        int pid, time_val;
        sscanf(line + strlen(first), "%d %d", &pid, &time_val);
        killList[killCount].pid = pid;
        killList[killCount].time = time_val;
        killCount++;
        return;
    }

    char name[MAX_NAME_LEN];
    char io1[16], io2[16];
    int pid, burst;

    int n = sscanf(line, "%s %d %d %s %s", name, &pid, &burst, io1, io2);

    if (n < 3)
    {
        return;
    }

    if (hashGet(pid))
    {
        return;
    }

    int io_start = -1, io_dur = 0;

    if (n == 5)
    {
        if (strcmp(io1, "-") != 0)
        {
            io_start = atoi(io1);
        }
        if (strcmp(io2, "-") != 0)
        {
            io_dur = atoi(io2);
        }
    }

    PCB *p = createPCB(name, pid, burst, io_start, io_dur);
    if (!p)
    {
        return;
    }

    procList[procCount++] = p;
    totalProcesses++;
    hashPut(pid, p);
    enqueue(&readyQ, p);
}

int hashFunction(int key)
{
    int h = key % HASH_MAP_SIZE;
    return (h < 0 ? h + HASH_MAP_SIZE : h);
}

void hashPut(int key, PCB *process)
{
    int idx = hashFunction(key);
    HashMapNode *cur = hashMap[idx];

    while (cur)
    {
        if (cur->key == key)
        {
            return;
        }
        cur = cur->next;
    }

    HashMapNode *node = (HashMapNode *)malloc(sizeof(HashMapNode));
    node->key = key;
    node->process = process;
    node->next = hashMap[idx];
    hashMap[idx] = node;
}

PCB *hashGet(int key)
{
    int idx = hashFunction(key);
    HashMapNode *cur = hashMap[idx];
    while (cur)
    {
        if (cur->key == key)
        {
            return cur->process;
        }
        cur = cur->next;
    }
    return NULL;
}

void initQueue(Queue *q)
{
    q->front = q->rear = NULL;
}

void enqueue(Queue *q, PCB *p)
{
    p->next = NULL;
    if (q->rear)
    {
        q->rear->next = p;
        q->rear = p;
    }
    else
    {
        q->front = q->rear = p;
    }
}

PCB *dequeue(Queue *q)
{
    if (!q->front)
    {
        return NULL;
    }
    PCB *p = q->front;
    q->front = p->next;
    if (!q->front)
    {
        q->rear = NULL;
    }
    return p;
}

PCB *removeFromQueue(Queue *q, int pid)
{
    PCB *cur = q->front;
    PCB *prev = NULL;
    while (cur)
    {
        if (cur->pid == pid)
        {
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                q->front = cur->next;
            }
            if (cur == q->rear)
            {
                q->rear = prev;
            }
            cur->next = NULL;
            return cur;
        }
        prev = cur;
        cur = cur->next;
    }
    return NULL;
}

void forEachQueue(Queue *q, void (*fn)(PCB *, void *), void *ctx)
{
    PCB *cur = q->front;
    while (cur)
    {
        PCB *nxt = cur->next;
        fn(cur, ctx);
        cur = nxt;
    }
}

PCB *createPCB(const char *name, int pid, int burst, int io_start, int io_dur)
{
    if (burst < 0 || io_dur < 0)
    {
        return NULL;
    }

    PCB *p = (PCB *)malloc(sizeof(PCB));
    strcpy(p->name, name);

    p->pid = pid;
    p->cpu_burst = burst;
    p->cpu_remaining = burst;

    p->io_start = io_start;
    p->io_duration = io_dur;
    p->io_remaining = 0;
    p->io_time_total = io_dur;

    p->executed_time = 0;
    p->completion_time = -1;
    p->killed = 0;
    p->killed_time = -1;
    p->state = READY;
    p->next = NULL;

    return p;
}

void decIO(PCB *p, void *ctx)
{
    if (p->io_remaining > 0)
    {
        p->io_remaining--;
    }
}

void checkIOCompletion()
{
    PCB *prev = NULL;
    PCB *cur = waitingQ.front;

    while (cur)
    {
        PCB *nxt = cur->next;
        if (cur->io_remaining == 0)
        {
            if (prev)
            {
                prev->next = cur->next;
            }
            else
            {
                waitingQ.front = cur->next;
            }
            if (cur == waitingQ.rear)
            {
                waitingQ.rear = prev;
            }
            cur->state = READY;
            enqueue(&readyQ, cur);
        }
        else
        {
            prev = cur;
        }
        cur = nxt;
    }
}

void terminatePCB(PCB *p, int time)
{
    p->state = TERMINATED;
    p->completion_time = time;
    enqueue(&terminatedQ, p);
}

void applyKillEvents(int time, PCB **running, int *terminated)
{
    for (int i = 0; i < killCount; i++)
    {
        if (killList[i].time == time)
        {
            int pid = killList[i].pid;
            PCB *target = hashGet(pid);
            if (!target || target->state == TERMINATED)
            {
                continue;
            }

            if (*running && (*running)->pid == pid)
            {
                PCB *p = *running;
                *running = NULL;
                p->killed = 1;
                p->killed_time = time;
                terminatePCB(p, time);
                (*terminated)++;
            }
            else
            {
                PCB *r = removeFromQueue(&readyQ, pid);
                if (!r)
                {
                    r = removeFromQueue(&waitingQ, pid);
                }
                if (r)
                {
                    r->killed = 1;
                    r->killed_time = time;
                    terminatePCB(r, time);
                    (*terminated)++;
                }
            }
        }
    }
}

void simulate()
{
    int t = 0;
    int terminated = 0;
    PCB *running = NULL;

    while (terminated < totalProcesses)
    {
        applyKillEvents(t, &running, &terminated);

        if (!running)
        {
            running = dequeue(&readyQ);
            if (running)
            {
                running->state = RUNNING;
            }
        }

        if (running)
        {
            running->executed_time++;
            running->cpu_remaining--;
        }

        forEachQueue(&waitingQ, decIO, NULL);

        if (running)
        {
            if (running->cpu_remaining > 0 &&
                running->io_start >= 0 &&
                running->executed_time == running->io_start &&
                running->io_duration > 0)
            {
                running->io_remaining = running->io_duration;
                running->state = WAITING;
                enqueue(&waitingQ, running);
                running = NULL;
            }
            else if (running->cpu_remaining == 0)
            {
                terminatePCB(running, t + 1);
                running = NULL;
                terminated++;
            }
        }

        checkIOCompletion();
        t++;
    }
}

int cmpPID(const void *a, const void *b)
{
    PCB *p1 = *(PCB **)a;
    PCB *p2 = *(PCB **)b;
    return p1->pid - p2->pid;
}

void printResults()
{
    PCB *arr[MAX_PROCS];
    int n = procCount;
    int anyKilled = 0;

    for (int i = 0; i < n; i++)
    {
        arr[i] = procList[i];
        if (arr[i]->killed)
        {
            anyKilled = 1;
        }
    }

    qsort(arr, n, sizeof(PCB *), cmpPID);

    if (!anyKilled)
    {
        printf("PID\tName\tCPU\tIO\tTurnaround\tWaiting\n");
        for (int i = 0; i < n; i++)
        {
            PCB *p = arr[i];
            int tat = p->completion_time;
            int wait = tat - p->cpu_burst;
            printf("%d\t%s\t%d\t%d\t%d\t\t%d\n", p->pid, p->name, p->cpu_burst, p->io_time_total, tat, wait);
        }
    }
    else
    {
        printf("PID\tName\tCPU\tIO\tStatus\t\tTurnaround\tWaiting\n");
        for (int i = 0; i < n; i++)
        {
            PCB *p = arr[i];
            if (p->killed)
            {
                printf("%d\t%s\t%d\t%d\tKILLED at %d\t-\t\t-\n", p->pid, p->name, p->cpu_burst, p->io_time_total, p->killed_time);
            }
            else
            {
                int tat = p->completion_time;
                int wait = tat - p->cpu_burst;
                printf("%d\t%s\t%d\t%d\tOK\t\t%d\t\t%d\n", p->pid, p->name, p->cpu_burst, p->io_time_total, tat, wait);
            }
        }
    }
}
