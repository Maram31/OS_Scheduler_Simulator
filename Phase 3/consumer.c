#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/msg.h>

/* Size of shared buffer */
#define BUF_SIZE 3

int add=0;										/* place to add next element */
int rem=0;										/* place to remove next element */
int num=0;										/* number elements in buffer */

union Semun
{
    int val;               /* value for SETVAL */
    struct semid_ds *buf;  /* buffer for IPC_STAT & IPC_SET */
    ushort *array;         /* array for GETALL & SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
    void *__pad;
};

void down(int sem)
{
    struct sembuf p_op;

    p_op.sem_num = 0;
    p_op.sem_op = -1;
    p_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &p_op, 1) == -1)
    {
        perror("Error in down()");
        exit(-1);
    }
}

void up(int sem)
{
    struct sembuf v_op;

    v_op.sem_num = 0;
    v_op.sem_op = 1;
    v_op.sem_flg = !IPC_NOWAIT;

    if (semop(sem, &v_op, 1) == -1)
    {
        perror("Error in up()");
        exit(-1);
    }
}

int main (int argc, char *argv[])
{
    key_t key_id = ftok("keyfile", 65);               
    int shmid = shmget(key_id, BUF_SIZE*sizeof(int), IPC_CREAT | 0644);
    printf("Consumer: Shared memory id: %d\n", shmid);

    union Semun semun;

    int producer_sem = semget(key_id, 1, 0666 | IPC_CREAT);
    key_id = ftok("keyfile", 60);
    int consumer_sem = semget(key_id, 1, 0666 | IPC_CREAT);
    key_id = ftok("keyfile", 70);
    int memory_sem = semget(key_id, 1, 0666 | IPC_CREAT);

    if (producer_sem == -1 || consumer_sem == -1 || memory_sem == -1)
    {
        perror("Consumer: Error in create sem");
        exit(-1);
    }

    semun.val = 0; /* initial value of the semaphore, Binary semaphore */
    if (semctl(producer_sem, 0, SETVAL, semun) == -1)
    {
        perror("Consumer: Error in semctl");
        exit(-1);
    }
    semun.val = BUF_SIZE;
    if (semctl(consumer_sem, 0, SETVAL, semun) == -1)
    {
        perror("Consumer: Error in semctl");
        exit(-1);
    }
    semun.val = 0;
    if (semctl(memory_sem, 0, SETVAL, semun) == -1)
    {
        perror("Consumer: Error in semctl");
        exit(-1);
    }

    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == -1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    printf("\nConsumer: Shared memory attached at address %x\n", shmaddr);
    while(1)
    {
        down(producer_sem);

        sleep(rand()%20+1);
        up(memory_sem);
        (*(int*)shmaddr)--;
        printf("Consumer: message recived with value %d\n", (*(int*)shmaddr));
        down(memory_sem);

        up(consumer_sem);
    }

	return 0;
}
