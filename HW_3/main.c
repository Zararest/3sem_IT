#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>

#define PARENT_DEATH_SIG 4

unsigned char cur_outp_byte = 0, cur_file_byte = 0;
unsigned char cur_bit_mask = 1;
int counter = 1;

#define CHECK(result)                                   \
    do {                                                \
        if (result < 0) {                               \
            printf ("Error in line: %d\n", __LINE__);   \
        }                                               \
    } while (0) 


void parent_death(int sig_id){

    printf("Parent is dead\n");
    exit(0);
}

void child_death(int sig_id){

    printf("Child is dead\n");
    exit(0);
}

void got_zero(int sig_id){

    cur_bit_mask = cur_bit_mask << 1;
    counter++;
}

void got_one(int sig_id){

    cur_outp_byte = cur_outp_byte | cur_bit_mask;
    cur_bit_mask = cur_bit_mask << 1;
    counter++;
}

void got_back(int sig_id){

}

void parent(pid_t child_id){ //
    
    struct sigaction act = {0};

    sigset_t set = {0};

    act.sa_handler = child_death;
    sigaction(SIGCHLD, &act, NULL);

    act.sa_handler = got_zero;
    sigaction(SIGUSR1, &act, NULL);

    act.sa_handler = got_one;
    sigaction(SIGUSR2, &act, NULL);
    
    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    sigemptyset(&set);

    while (1){

        sigsuspend(&set);  //конец первой //конец второй 
        
        if (counter > 8){

            write(STDOUT_FILENO, &cur_outp_byte, 1);
            cur_outp_byte = 0;
            cur_bit_mask = 1;
            counter = 1;
        }
        
        kill(child_id, SIGUSR1); //начало второй
    }
}

void child(int fd, int real_parent_id){

    struct sigaction act = {0};

    sigset_t set = {0};

    act.sa_handler = parent_death;
    prctl(PR_SET_PDEATHSIG, PARENT_DEATH_SIG);    //вроде здесь
    sigaction(PARENT_DEATH_SIG, &act, NULL);

    act.sa_handler = got_back;
    sigaction(SIGUSR1, &act, NULL);

    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    sigemptyset(&set);

    if (getppid() != real_parent_id){

        printf("Real parent is dead\n");
        exit(0);
    }

    int parent_id = getppid();

    while (read(fd, &cur_file_byte, 1) > 0){

        cur_bit_mask = 1;
        for (int i = 0; i < 8; i++){

            if ((cur_bit_mask & cur_file_byte) == 0){
                
                CHECK(kill(parent_id, SIGUSR1));
            } else{

                CHECK(kill(parent_id, SIGUSR2));
            }
            
            cur_bit_mask = cur_bit_mask << 1;
            
            sigsuspend(&set); //конец первой //начало второй //конец второй 
        }
    }
}

/*
    Крит секции:
        1) в родителе от форка до сигсаспенда гонка с ребенком. борьба за маску пришедших сигналов     
        2) в родителе гонка с ребенком от sigsuspend до sigsuspend. борьба за маску пришедших сигналов got_one got_zero
        3) в ребенке от kill до sigsuspend гонка с родителем за маску пришедших сигналов ребенка       got_back
        4) в ребенке гонка от fork до prctl c системой за маску пришедших сигналов                    
        5) в родителе гонка за глобаьные переменные между обработчиками 
        6) в родителе гонка между обработчиками и телом родителя за глобальные переменные от sigsuspend до sigsuspend  
*/


int open_file(char* file){

    int tmp = open(file, O_RDONLY);

    if (tmp == -1){

        printf("Can't open file\n");
        exit(0);
    }

    return tmp;
}

int main(int argc, char* argv[]){

    if (argc < 2){

        printf("Not enogth args\n"); 
        exit(0);
    }

    pid_t cur_parent = getpid();

    sigset_t set_of_blocked_signals = {0};
    
    struct sigaction act = {0};

    sigaddset(&set_of_blocked_signals, SIGUSR1);
    sigaddset(&set_of_blocked_signals, SIGUSR2);
    sigaddset(&set_of_blocked_signals, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set_of_blocked_signals, NULL); //переносим sigproc mask после форк и при этом сигактион определены что будет?

    pid_t child_id = fork(); //если переместить sigprocmask после fork, то:
                             //ребенок отправляет сигнал. Родитель его обрабатывает до sigsuspend. Потом встает на нем тк ребенок ждет подтверждение.

    if (child_id == 0){

        child(open_file(argv[1]), cur_parent);
        printf("The end of child\n");
    } else{

        printf("Child id: %d Par id: %d\n", child_id, getpid());
        parent(child_id);
    }
}
