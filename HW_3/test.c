/*#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/prctl.h>
#include <signal.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>

void handler(int sig_num){

    printf("hadled\n");
    exit(0);
}

void parent(pid_t real_parent){

    sleep(2);
    exit(0);
}

void child(){

    struct sigaction act = {.sa_handler = handler,};
    prctl(PR_SET_PDEATHSIG, 1);
    
    int signal_num = -1;

    printf("sig_action = %i\n", sigaction(1, &act, NULL));
    sleep(4);
    printf("end of child\n"); 
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

    pid_t cur_parent = getpid();
    pid_t child_id = fork();

    if (child_id == 0){

        child();
    } else{

        parent(cur_parent);
    }
}*/

#ifndef BUG_BASIC_XSTRING_HPP
#define BUG_BASIC_XSTRING_HPP

// #include "basic_xstring.hpp"

namespace meta {

template <typename CharT>
class buf_basic_string {
using size_type = basic_xstring <CharT>::;

// CharT* data_;
// size_type size_;
// public:
// buf_basic_string (size_type size) {}
};

}

#endif // BUG_BASIC_XSTRING_HPP