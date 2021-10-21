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

    printf("unlink 1 = %i\n", unlink("./fifo/test_fifo"));
    printf("unlink 2 = %i\n", unlink("./fifo/test_fifo"));

    write(tmp_fifo, "11", 2);
    printf("read: %li\n", read(tmp_fifo, str, 1));

    mkfifo("./fifo/test_fifo", 0777);
    int second_fifo = open("./fifo/test_fifo", O_RDWR);
    printf("attempt to open %i\n", second_fifo);
    
    write(second_fifo, "22", 2);
    printf("read 2: %li\n", read(second_fifo, str, 3));
}