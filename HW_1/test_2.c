#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(){

    char str[100], outp_1[100], outp_2[100];

    mkfifo("./fifo/test_fifo", 0777);

    printf("test 2\n");

    int tmp_fifo = open("./fifo/test_fifo", O_RDONLY);
    printf("after open |%i|\n", tmp_fifo);

    fgetc(stdin);
    close(tmp_fifo);

    printf("end\n");
}