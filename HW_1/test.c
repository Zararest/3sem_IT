#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
 #include <signal.h>

int main(){

    char str[100], outp_1[100], outp_2[100];

    mkfifo("./fifo/test_fifo", 0777);

    printf("test 1\n");
    int tmp_fifo = open("./fifo/test_fifo", O_RDWR);

    int tmp_fifo_write = open("./fifo/test_fifo", O_RDONLY);

    close(tmp_fifo);
    printf("after close ok\n");

    printf("before sigpipe\n");
    read(tmp_fifo_write, outp_1, 1);
    printf("after sigpipe\n");
}