#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>



void producer(int fd){

    //семафор UNDO  для входа 

        //семафор со значением процесса с UNDO
        //пока файл полность не прочитали исполняем 
            //empty семафор с UNDO
            //заполняем память (передаем по одному блоку)
            //full семафор с UNDO
        //закончили запись
    //семафор на то что другой процесс закончил вывод(вроде можно empty использовать)
    //сменяем 
        
}

void consumer(){

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