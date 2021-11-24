#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <linux/kcmp.h>
#include <sys/syscall.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define MAXLEN 200

int is_the_same_OFD(int fir_fd, int sec_fd){

    int ret = syscall(SYS_kcmp, getpid(), getpid(), KCMP_FILE, fir_fd, sec_fd);

    if (ret == -1){

        printf("error = %i\n", errno);
        return -1;
    }

    if (ret == 0){

        return 1;
    }

    return 0;
}

int is_the_same_stat(int fir_fd, int sec_fd){

    struct stat fir_st, sec_st;

    if (fstat(fir_fd, &fir_st) == -1){

        printf("first error = %i\n", errno);
        return 0;
    }

    if (fstat(sec_fd, &sec_st) == -1){

        printf("sec error = %i\n", errno);
        return 0;
    }

    printf("1_inode %li 2_inod %li\n", fir_st.st_ino, sec_st.st_ino);
    printf("1_device %li 2_device %li\n", fir_st.st_dev, sec_st.st_dev);

    if (fir_st.st_ino == sec_st.st_ino){

        return 1;
    }

    return 0;
}

int is_the_same(int fir_fd, int sec_fd){

    char path[MAXLEN];
    char fir_name[MAXLEN], sec_name[MAXLEN];
    int size_of_fir_name = 0, size_of_sec_name = 0;

    sprintf(path,"/proc/%d/fd/%d", getpid(), fir_fd);

    if ((size_of_fir_name = readlink(path, fir_name, MAXLEN)) == -1){

        printf("errno 1 = %i", errno);
        return 0;
    }
    fir_name[size_of_fir_name] = '\0';

    sprintf(path,"/proc/%d/fd/%d", getpid(), sec_fd);

    if ((size_of_sec_name = readlink(path, sec_name, MAXLEN)) == -1){

        printf("errno 2 = %i", errno);
        return 0;
    }
    sec_name[size_of_sec_name] = '\0';


    printf("fir = |%s| sec = |%s|\n", fir_name, sec_name);
    if (strcmp(fir_name, sec_name) == 0){

        return 1;
    }

    return 0;
}

#define CHECK(result)                                   \
    do {                                                \
        if (result < 0) {                               \
            printf ("Error in line: %d\n", __LINE__);   \
            return -1;                                  \
        }                                               \
    } while (0)

int main(){

    char path[MAXLEN];
    char buf[MAXLEN];
    /*int fir_fd = open("./fifo/test_fifo", O_RDWR);
    unlink("./fifo/test_fifo");
    mkfifo("./fifo/test_fifo", 0777);
    int sec_fd = open("./fifo/test_fifo", O_RDWR);
    unlink("./fifo/test_fifo");
    mkfifo("./fifo/test_fifo", 0777);
    int trd_fd = open("./fifo/test_fifo", O_RDWR);*/

    int fir_fd = open("./fifo/test", O_RDWR | O_CREAT);
    int thr = open("./fifo/test", O_RDWR);
    printf("remove = %d\n", remove("./fifo/test"));

    int sec_fd = open("./fifo/test.txt", O_RDWR | O_CREAT | 0666);
    CHECK (sec_fd);
    CHECK (unlink("./fifo/test.txt"));

    printf("%d\n", write(fir_fd, "hello", 5));
    CHECK (lseek (fir_fd, 0, SEEK_SET));
    CHECK (lseek (sec_fd, 0, SEEK_SET));
    printf("len = %i\n", read(sec_fd, buf, 5));

    printf("is the same = %d\n", is_the_same_stat(fir_fd, sec_fd));
}