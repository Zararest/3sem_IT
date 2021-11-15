#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>

#define MEMORY_KEY 42
#define SEMAPHORES_KEY 44
#define SIZE_OF_BUF 10
#define NUM_OF_SEMS 6

//не откатывается full
enum Semaphore{ //для каждого процесса свое значение отката

    mutex_prod = 0,
    full = 1,
    empty = 2,
    prod_id = 3,
    cons_id = 4,
    mutex_cons = 5
};

struct Buf_data{

    int data_size;
    char data[SIZE_OF_BUF];
};

struct sembuf init_op(unsigned short num, short op, short flag){

    struct sembuf tmp;
    tmp.sem_num = num;
    tmp.sem_op =  op;
    tmp.sem_flg = flag;
    
    return tmp;
}

int create_semaphores(){

    int sem_id = semget(SEMAPHORES_KEY, NUM_OF_SEMS, IPC_CREAT | IPC_EXCL | 0777);

    if (sem_id == -1){

        sem_id = semget(SEMAPHORES_KEY, NUM_OF_SEMS, IPC_CREAT | 0777);
        if (sem_id == -1){

            printf("Can't open semaphores error[%i]\n", errno);
            exit(0);
        }

        return sem_id; 
    } else{

        struct sembuf sems_arr[NUM_OF_SEMS];

        if ((semctl(sem_id, mutex_prod, SETVAL, 1) == -1) || (semctl(sem_id, mutex_cons, SETVAL, 1) == -1)){

            printf("Can't init sem\n");
            exit(0);
        }
        
        return sem_id;
    }
}

int create_shared_m(){

    int shm_id = shmget(MEMORY_KEY, sizeof(struct Buf_data), IPC_CREAT | 0777);

    if (shm_id == -1){

        printf("Can't create shared memory error[%i]\n", errno);
        exit(0);
    }
    return shm_id;
}

void write_to_shm(int shm_id, struct Buf_data* data_from_file){

    void* shm_ptr = shmat (shm_id, 0, 0);

    if (shm_ptr == NULL){

        printf("Can't access shared memory in write error[%i]\n", errno);
        exit(0);
    }
    memcpy(shm_ptr, data_from_file, sizeof(struct Buf_data));
    shmdt(shm_ptr);
}

int read_from_shm(int shm_id, struct Buf_data* buf){

    void* shm_ptr = shmat (shm_id, 0, 0);

    if (shm_ptr == NULL){

        printf("Can't access shared memory in read error[%i]\n", errno);
        exit(0);
    }
    memcpy(buf, shm_ptr, sizeof(struct Buf_data));
    shmdt(shm_ptr);
}


void producer(int fd){

    struct Buf_data data_from_file;
    int my_consumer_id = 0;
    int sem_id = create_semaphores();
    int shm_id = create_shared_m();
    struct sembuf sem_op;

    sem_op = init_op(mutex_prod, -1, SEM_UNDO); //тут встают процессы кроме одного
    semop(sem_id, &sem_op, 1);
//-----------
    sem_op = init_op(prod_id, getpid() % 10000, SEM_UNDO);    
    semop(sem_id, &sem_op, 1);
  
    while ((my_consumer_id = semctl(sem_id, cons_id, GETVAL, NULL)) == 0){}
    
    do{
        data_from_file.data_size = read(fd, data_from_file.data, SIZE_OF_BUF);
        sleep(1);
        sem_op = init_op(empty, -1, 0);    
        semop(sem_id, &sem_op, 1);
    
        if (my_consumer_id == semctl(sem_id, cons_id, GETVAL, NULL)){

            write_to_shm(shm_id, &data_from_file);
        } else{
            
            printf("Other consumer now\n");
            exit(0);
        }
        
        sem_op = init_op(full, 1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);

    } while (data_from_file.data_size != 0);

    while (my_consumer_id == semctl(sem_id, cons_id, GETVAL, NULL)){}

    sem_op = init_op(prod_id, -(getpid() % 10000), SEM_UNDO);    
    semop(sem_id, &sem_op, 1);
//-----------   
    sem_op = init_op(mutex_prod, 1, SEM_UNDO);
    semop(sem_id, &sem_op, 1);
}


void consumer(){

    struct Buf_data data_from_shm;
    data_from_shm.data_size = -1;
    int my_prod_id = 0, op_ret = 0;
    int sem_id = create_semaphores();
    int shm_id = create_shared_m();
    struct sembuf sem_op;

    sem_op = init_op(mutex_cons, -1, SEM_UNDO); //тут встают процессы кроме одного
    semop(sem_id, &sem_op, 1);
//-----------  
    sem_op = init_op(cons_id, getpid() % 10000, SEM_UNDO);    
    semop(sem_id, &sem_op, 1);
    
    while ((my_prod_id = semctl(sem_id, prod_id, GETVAL, NULL)) == 0){}

    if (semctl(sem_id, empty, SETVAL, 1) == -1){

        printf("Can't set empty sem\n");
        exit(0);
    }

    while (data_from_shm.data_size != 0){
        sleep(1);
        do{
            sem_op = init_op(full, -1, O_NONBLOCK);    
            op_ret = semop(sem_id, &sem_op, 1);
            if (my_prod_id != semctl(sem_id, prod_id, GETVAL, NULL)){

                printf("Other producer now\n");
                exit(0);
            }
        } while(op_ret == -1);

        if (my_prod_id == semctl(sem_id, prod_id, GETVAL, NULL)){

            read_from_shm(shm_id, &data_from_shm);
        } else{

            printf("Other producer now\n");
            exit(0);
        }

        if (my_prod_id == semctl(sem_id, prod_id, GETVAL, NULL)){

            write(STDOUT_FILENO, data_from_shm.data, data_from_shm.data_size);
        } else{

            printf("Other producer now\n");
            exit(0);
        }
        
        sem_op = init_op(empty, 1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);
    }
    sem_op = init_op(cons_id, -(getpid() % 10000), SEM_UNDO);    
    semop(sem_id, &sem_op, 1);
//-----------  
    sem_op = init_op(mutex_cons, 1, SEM_UNDO); 
    semop(sem_id, &sem_op, 1);
}

int open_file(char* file){

    int tmp = open(file, O_RDONLY);

    if (tmp == -1){

        printf("Can't open file\n");
        exit(0);
    }

    return tmp;
}

int main(int argc, char* argv[]){//ipcs - посмотреть очередь сообщений, семафоры и память 

    if (argc == 1){

        consumer();
    } else{

        producer(open_file(argv[1]));
    }

}