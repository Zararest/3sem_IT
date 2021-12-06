#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NUM_OF_SEMS 7

struct sembuf init_op(short num, short op, short flag){

    struct sembuf tmp;
    tmp.sem_num = num;
    tmp.sem_op = op;
    tmp.sem_flg = flag;
    
    return tmp;
}

int main(){

    struct sembuf sems_arr;

    int sem_id = semget(44, NUM_OF_SEMS, IPC_CREAT | 0777);

    struct sembuf sem_op[4];
    printf("sem id = %i\n", sem_id);
    printf("error after create = %i\n", errno);
    
    for (int i = 0; i < 7; i++){

        printf("[%i]: %i\n", i, semctl(sem_id, i, GETVAL));
    }


   /* pid_t child = fork();

    if (child == 0){

        //sem_op[0] = init_op(0, 0, 0);
        sem_op[0] = init_op(0, -3, IPC_NOWAIT);
        sem_op[1] = init_op(0, 0, IPC_NOWAIT);
        sem_op[2] = init_op(0, 3, 0);
        sem_op[3] = init_op(0, 0, 0);
        semop(sem_id, sem_op, 4);
        printf("in child\n");
    } else{

        sleep(1);

        for (int i = 0; i < NUM_OF_SEMS; i++){
        
            printf("before %i: %i\n", i, semctl(sem_id, i, GETVAL, 0));
        }

        sem_op[0] = init_op(0, 1, 0);
        semop(sem_id, sem_op, 1);

        for (int i = 0; i < NUM_OF_SEMS; i++){
        
            printf("after %i: %i\n", i, semctl(sem_id, i, GETVAL, 0));
        }
    }

    printf("delete = %i\n", semctl(42, 0, IPC_RMID, NULL));*/
}