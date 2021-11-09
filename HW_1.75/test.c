#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NUM_OF_SEMS 4

struct sembuf init_op(short num, short op, short flag){

    struct sembuf tmp;
    tmp.sem_num = num;
    tmp.sem_op = op;
    tmp.sem_flg = flag;
    
    return tmp;
}

int main(){

    struct sembuf sems_arr;
    key_t key = 2;
    int sem_id = semget(key, NUM_OF_SEMS, IPC_CREAT | 0777);
    printf("sem id = %i\n", sem_id);
    printf("error after create = %i\n", errno);

    /*for (int i = 0; i < NUM_OF_SEMS; i++){

        printf("sem_num[%i] = %i\n", i, semctl(sem_id, i, GETVAL, NULL));
    }
    sems_arr = init_op(1, 1, SEM_UNDO);
    semop(sem_id, &sems_arr, 1);

    sems_arr = init_op(1, -2, SEM_UNDO);
    semop(sem_id, &sems_arr, 1);

    for (int i = 0; i < NUM_OF_SEMS; i++){

        printf("after sem_num[%i] = %i\n", i, semctl(sem_id, i, GETVAL, NULL));
    }*/

    printf("delete = %i\n", semctl(42, 0, IPC_RMID, NULL));
}