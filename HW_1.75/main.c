#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>

#define SIZE_OF_BUF 99
#define NUM_OF_SEMS 6
#define KEY 10

enum Semaphore{

    mutex_prod = 0,
    full = 1,
    empty = 2,
    prod_id = 3,
    cons_id = 4,
    mutex_cons = 5
};

struct sembuf init_op(int num, int op, int flag){

    struct sembuf tmp;
    tmp.sem_num = num;
    tmp.sem_op = op;
    tmp.sem_flg = flag;

    return tmp;
}

int create_semaphores(){

    int sem_id = semget(KEY, NUM_OF_SEMS, IPC_CREAT | IPC_EXCL | 0777);

    if (sem_id == -1){

        return semget(KEY, NUM_OF_SEMS, IPC_CREAT | 0777);
    } else{

        struct sembuf sems_arr[NUM_OF_SEMS];
        sems_arr[mutex_prod] = init_op(mutex_prod, 1, 0);
        sems_arr[mutex_cons] = init_op(mutex_cons, 1, 0);
        sems_arr[full] = init_op(full, 0, 0);
        sems_arr[empty] = init_op(empty, 1, 0);
        sems_arr[prod_id] = init_op(prod_id, 0, 0);
        sems_arr[cons_id] = init_op(cons_id, 0, 0);

        semop(sem_id, sems_arr, NUM_OF_SEMS);

        if (errno != 0){

            printf("Init problem = %i\n", errno);
            exit(0);
        }
    }

    return sem_id;
}

void write_to_shm(int fd, char* data_from_file, int readed_info_size){

}

int read_from_shm(char* buffer){


}


void producer(int fd){

    //семафор UNDO  для входа 

        //семафор со значением процесса с UNDO (аналогичный для read)
        //пока файл полность не прочитали исполняем 
            //empty семафор с UNDO
            //заполняем память (передаем по одному блоку)
            //full семафор с UNDO
        //закончили запись
    //семафор на то что другой процесс закончил вывод(вроде можно empty использовать)
    //сменяем семафор для входа   
    char data_from_file[SIZE_OF_BUF];
    int my_consumer_id = 0, readed_data_size = 0;
    int sem_id = create_semaphores();
    struct sembuf sem_op;

    sem_op = init_op(mutex_prod, -1, SEM_UNDO); //тут встают процессы кроме одного
    semop(sem_id, &sem_op, 1);
//-----------
    sem_op = init_op(prod_id, pid(), SEM_UNDO);    
    semop(sem_id, &sem_op, 1);

    while ((my_consumer_id = semctl(sem_id, cons_id, GETVAL, NULL)) == 0){}

    while ((readed_data_size = read(fd, data_from_file, SIZE_OF_BUF)) != 0){ //тут надо поправить случаи когда файл ровно делится на сегменты
 
        sem_op = init_op(empty, -1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);

        if (my_consumer_id == semctl(sem_id, cons_id, GETVAL, NULL)){

            write_to_shm(fd, data_from_file, readed_data_size);
        } else{
            
            printf("Other consumer now\n");
            exit(0);
        }
        
        sem_op = init_op(full, 1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);
    }
    sem_op = init_op(empty, -1, SEM_UNDO);    
    semop(sem_id, &sem_op, 1);//надо подумать

    sem_op = init_op(prod_id, -pid(), SEM_UNDO);    
    semop(sem_id, &sem_op, 1);

    sem_op = init_op(empty, 1, SEM_UNDO);    
    semop(sem_id, &sem_op, 1);//надо подумать
//-----------   
    sem_op = init_op(mutex_prod, 1, SEM_UNDO);
    semop(sem_id, &sem_op, 1);
}


void consumer(){

    char data_from_shm[SIZE_OF_BUF];
    int my_prod_id = 0, readed_data_size = SIZE_OF_BUF;
    int sem_id = create_semaphores();
    struct sembuf sem_op;

    sem_op = init_op(mutex_cons, -1, SEM_UNDO); //тут встают процессы кроме одного
    semop(sem_id, &sem_op, 1);
//-----------  
    sem_op = init_op(cons_id, pid(), SEM_UNDO);    
    semop(sem_id, &sem_op, 1);

    while ((my_prod_id = semctl(sem_id, cons_id, GETVAL, NULL)) == 0){}

    while (readed_data_size == SIZE_OF_BUF){

        sem_op = init_op(full, -1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);

        if (my_prod_id == semctl(sem_id, cons_id, GETVAL, NULL)){

            readed_data_size = read_from_shm(data_from_shm);
        } else{

            printf("Other producer now\n");
            exit(0);
        }

        if (my_prod_id == semctl(sem_id, cons_id, GETVAL, NULL)){

            write(stdout, data_from_shm, readed_data_size);
        } else{

            printf("Other producer now\n");
            exit(0);
        }

        sem_op = init_op(empty, 1, SEM_UNDO);    
        semop(sem_id, &sem_op, 1);
    }
    sem_op = init_op(cons_id, -pid(), SEM_UNDO);    
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