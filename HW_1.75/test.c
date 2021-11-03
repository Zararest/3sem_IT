#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>

#define NUM_OF_SEMS 4

int main(){

    struct sembuf sems_arr[NUM_OF_SEMS];
    key_t key = 2;
    int sem_id = semget(key, NUM_OF_SEMS, IPC_CREAT | 0777);
    printf("sem id = %i\n", sem_id);
    printf("error after create = %i\n", errno);

    for (int i = 0; i < NUM_OF_SEMS; i++){

        printf("sem_num[%i] = %i\n", i, sems_arr[i].sem_num);
    }

    pid_t child_id = fork();

    if (child_id == 0){

        sems_arr[0].sem_num = 1;
        sems_arr[0].sem_op = 5;
        sems_arr[0].sem_flg = SEM_UNDO;

        printf("op in child = %i\n", semop(sem_id, sems_arr, NUM_OF_SEMS));
        printf("errno in child = %i\n", errno);

        sems_arr[0].sem_num = 0;
        sems_arr[0].sem_op = -1;
        sems_arr[0].sem_flg = SEM_UNDO;

        printf("op inc in child = %i\n", semop(sem_id, sems_arr, NUM_OF_SEMS));
        printf("errno in child = %i\n", errno);
        exit(0); //проверяем откат через UNDO
    } else{

        sleep(1);
        printf("sem val = %i\n", semctl(sem_id, 0, GETVAL, NULL));
    }   
    sleep(2);
}