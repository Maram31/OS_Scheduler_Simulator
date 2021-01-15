#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/msg.h>
#include <signal.h>

/* Size of shared buffer */
#define BUF_SIZE 3
int buffer[BUF_SIZE];

int add=0;										/* place to add next element */
int rem=0;										/* place to remove next element */
int num=0;										/* number elements in buffer */

#define MUTEX 0
#define FULL 1
#define EMPTY 2

int semid;

union semun {
	int val; /*	Value	for	SETVAL	*/
	struct semid_ds *buf;		/*	Buffer	for	IPC_STAT,	IPC_SET	*/
	unsigned short *array;	/*	Array	for	GETALL,	SETALL	*/
	struct seminfo *__buf;	/*	Buffer	for	IPC_INFO (Linux-specific)	*/
};

int sem_create(int nsems) { 
	int  id;
	key_t key = 1234;
	int semflg = IPC_CREAT | 0666;
	id = semget(key, nsems, semflg);
	if(id < 0)
	{
		perror("semget:");
		exit (1);
	}
	return id;
}

void sem_initialise(int semno, int val) {
	union semun un;
	un.val = val;
	if(semctl(semid, semno, SETVAL, un) < 0)
	{
	//	printf("%d\n", semno);
		perror("semctl:");
		exit(2);
	}
}

void down(int semno) {
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = -1;
	buf.sem_flg = 0;
	if(semop(semid, &buf, 1) < 0) {
		perror("semop:");
		exit(2);
	}
}

void up(int semno) {
	struct sembuf buf;
	buf.sem_num = semno;
	buf.sem_op = 1;
	buf.sem_flg = 0;
	if(semop(semid, &buf, 1) < 0)
	{
		perror("semop:");
		exit(2);
	}
}

void handler(int signum);
int shmid_resources;
int shmid;
int shmid_num;

void *shmaddr_resources;
int main (int argc, char *argv[])
{
    signal(SIGINT, handler);
    signal(SIGTSTP, handler);
    signal(SIGKILL, handler);

    key_t key_id = ftok("keyfile", 80);               
    shmid_resources = shmget(key_id, BUF_SIZE*sizeof(int), IPC_CREAT | 0644);

    shmaddr_resources = shmat(shmid_resources, (void *)0, 0);
    if (shmaddr_resources == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    

    key_id = ftok("keyfile", 65);               
    shmid = shmget(key_id, BUF_SIZE*sizeof(int), IPC_CREAT | 0644);
    //printf("Consumer: Shared memory id: %d\n", shmid);

    key_id = ftok("keyfile", 60);               
    shmid_num = shmget(key_id, sizeof(int), IPC_CREAT | 0644);
    //printf("Consumer: Shared memory num id: %d\n", shmid_num);

    semid = sem_create(3);
	if((*(int*)shmaddr_resources) == 0)
    {
        sem_initialise(MUTEX, 1);
        sem_initialise(FULL, 0);
        sem_initialise(EMPTY, BUF_SIZE);
    }

    void *shmaddr = shmat(shmid, (void *)0, 0);
    if (shmaddr == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }

    void *shmaddr_num = shmat(shmid_num, (void *)0, 0);
    if (shmaddr == (void*)-1)
    {
        perror("Error in attach in consumer");
        exit(-1);
    }
    (*(int*)shmaddr_resources) ++;

    int i;
    while(1)
    {
        if( (*(int*)shmaddr_num)> 0)
        {
            down(FULL);
            sleep(rand()%10+1);

            down(MUTEX);
            i = (*((int*)shmaddr+ rem));

            if(i != -1)
            {
                (*(int*)shmaddr_num) --;
                (*((int*)shmaddr+ rem)) = -1;                
                printf("Consumer: message recived with value %d\n", i);
            }

            rem = (rem+1) % BUF_SIZE;

            int j;
            for(j = 0; j<BUF_SIZE; j++)
            {
                printf("%d\t", (*((int*)shmaddr+ j)));
            }
            printf("\n");

            up(MUTEX);
            up(EMPTY);
        }
    }

	return 0;
}

void handler(int signum)
{
    (*(int*)shmaddr_resources) --;
    printf("Resources: %d\n", (*(int*)shmaddr_resources));
    if((*(int*)shmaddr_resources) == 0)
    {
        shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_num, IPC_RMID, (struct shmid_ds *)0);
        shmctl(shmid_resources, IPC_RMID, (struct shmid_ds *)0);
        semctl(semid, 0, IPC_RMID);    
        
    }
    raise(SIGTERM);                      
}