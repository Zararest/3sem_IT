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

#define PARENT_DEATH_SIG 1

unsigned char cur_outp_byte = 0, cur_file_byte = 0;
unsigned char cur_bit_mask = 0;
int counter = 0;

void parent_death(int sig_id){

    printf("Parent is dead\n");
    exit(0);
}

void child_death(int sig_id){

    printf("Child is dead\n");
    exit(0);
}

void got_zero(int sig_id){

}

void got_one(int sig_id){

}

void got_back(int sig_id){

}

void parent(pid_t child_id){

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));

    sigset_t set;
    memset(&set, 0, sizeof(sigset_t));

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

        sigsuspend(&set);

        if (counter >= 8){

            printf("%c", cur_outp_byte);
            counter = 0;
            cur_outp_byte = 0;
            cur_bit_mask = 0;
        }
        kill(child_id, SIGUSR1);
    }
}

void child(int fd, int real_parent_id){

    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));

    sigset_t set;
    memset(&set, 0, sizeof(sigset_t));

    act.sa_handler = parent_death;
    prctl(PR_SET_PDEATHSIG, PARENT_DEATH_SIG);
    sigaction(PARENT_DEATH_SIG, &act, NULL);

    sigaddset(&set, SIGCHLD);
    sigprocmask(SIG_UNBLOCK, &set, NULL);
    sigemptyset(&set);

    if (getppid() != real_parent_id){

        printf("Real parent is dead\n");
        exit(0);
    }

    int parent_id = getppid();

    while (read(fd, &cur_file_byte, 1) != 0){

        cur_bit_mask = 1;
        for (int i = 0; i < 8; i++){

            if (cur_bit_mask & cur_file_byte == 0){

                kill(parent_id, SIGUSR1);
            } else{

                kill(parent_id, SIGUSR2);
            }
            cur_bit_mask = cur_bit_mask << 1;
            sigsuspend(&set);
        }
    }

}

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

    sigset_t set_of_blocked_signals;
    memset(&set_of_blocked_signals, 0, sizeof(sigset_t));
    
    struct sigaction act;
    memset(&act, 0, sizeof(struct sigaction));

    sigaddset(&set_of_blocked_signals, SIGUSR1);
    sigaddset(&set_of_blocked_signals, SIGUSR2);
    sigaddset(&set_of_blocked_signals, SIGCHLD);
    sigprocmask(SIG_BLOCK, &set_of_blocked_signals, NULL);

    pid_t child_id = fork();

    if (child_id == 0){

        child(open_file(argv[1]), cur_parent);
    } else{

        parent(child_id);
    }
}
