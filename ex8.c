#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
union semun {
   int val;                     /* value for SETVAL */
   struct semid_ds *buf;        /* buffer for IPC_STAT, IPC_SET */
   unsigned short *array;  /* array for GETALL, SETALL */
     /* tableau de taille egale au nombre de semaphores de l'ensemble */
   struct seminfo *__buf;       /* buffer for IPC_INFO */
} arg;

int main(int argc , char* argv[]){
	arg.val = atoi(argv[2]);
	key_t key = ftok("main",'T');
	if(key == -1){
		perror("key");
		exit(0);
	}
	int semid = semget(key, 1 , IPC_CREAT | 0644);
	if(semid == -1){
		perror("semid");
		exit(0);
	}
	struct sembuf *sops = (struct sembuf*)malloc(sizeof(struct sembuf));
	sops->sem_num = 0;        /* Agir sur le sémaphore 3 */
        sops->sem_op = -1;         /* decremente de 1 */
        sops->sem_flg = 0;
        int se = semctl(semid, 0 , SETVAL , arg);
        if(se == -1){
        	perror("semctl");
        	exit(0);
        }
        int val = atoi(argv[1]);
        pid_t pid[val];
        int i;
        for(i=0;i < val; i++){
        	pid[i] = fork();
        	if(pid[i] == 0){
        		int semid = semget(key, 1, 0644);
			if(semid == -1){
				perror("semget");
				exit(0);
			}
        		printf("création du mineur %d %d\n",i+1,getpid());
        		fflush(stdout);
        		srand(getpid()*time(NULL));
        		int n = rand()%5;
        		printf("le mineur %d attend %d heure avant de travailler \n",i+1 , n);
        		fflush(stdout);
        		sleep(n);
			semop(semid, sops, 1);
        		printf("le mineur %d est entrain de travailler et prend les outils  \n",i+1);
        		fflush(stdout);
        		n = rand()%5;
        		sleep(n);
        		printf("le mineur %d a fini de travailler et rend les outils \n",i+1);
        		fflush(stdout);
        		sops->sem_op = 1;
        		semop(semid, sops, 1);
        		exit(0); 
        	}
        }
        while(waitpid(-1,NULL,0)!=-1)
        		sleep(1);
        semctl(semid, 0, IPC_RMID);
        
        printf("fin du travail des mineurs \n");
}	
        	
