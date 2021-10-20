#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int main(){

    char str[100], outp_1[100], outp_2[100];
    
    int fir_file = open("test.txt", O_RDONLY);
    int sec_file = open("test.txt", O_RDONLY);

    read(fir_file, outp_1, 10);
    read(sec_file, outp_2, 10);

    printf("fir |%s| sec |%s|\n", outp_1, outp_2);

    /*unlink("./fifo/test_fifo");
    printf("make fifo = |%i|\n", mkfifo("./fifo/test_fifo", S_IFIFO | 0777));
    scanf("%s", str);

    int inp_fifo = open("./fifo/test_fifo", O_RDWR);
    printf("here\n");
    int fir_read = open("./fifo/test_fifo", O_RDONLY);
    int sec_read = open("./fifo/test_fifo", O_RDONLY);

    printf("write = |%li|\n", write(inp_fifo, str, 6));
    printf("read fir = |%li|\n", read(fir_read, outp_1, 2));
    printf("{%s}\n", outp_1);
    printf("read sec = |%li|\n", read(sec_read, outp_2, 2));
    printf("{%s}\n", outp_2);*/
}