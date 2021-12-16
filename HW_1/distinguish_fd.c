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

/*Флешка:
    Пока существует открытый файловый дескриптор информация не будет удалена и свободного места не прибавится 
*/

/*Задача с фифо:

    При открытии файла 666 воспринимается как десятичное число, а не восьмеричное число. Из-за этого права доступа интерпритируются как:
    01232
    Программа выведет на экран, если:
    - существут fifo с правами доступа, разрешающими данному процессу доступ
    - данный процесс обладает правами суперпользователя
*/

#define MAXLEN 200

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

    if ((fir_st.st_ino == sec_st.st_ino) && (fir_st.st_dev == sec_st.st_dev)){

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

/*Случаи, когда функция вернет 0:
    1) случился unlink(удаление ссылки) файла и создался новый файл
    2) случился mount (271 страница)
    3) файл был переименован и создан новый с тем же именем
*/

int main(){

    char path[MAXLEN];
    char buf[MAXLEN];

    int fir_fd = open("./test.txt", O_RDWR | O_CREAT);
    fgetc(stdin);
    //int sec_fd = open("./fifo/link", O_RDWR);
 
    CHECK(fir_fd);
    //CHECK(sec_fd);

    printf("write = %li\n", write(fir_fd, "LOL", 3));
    printf("read = %li\n", read(fir_fd, buf, 3));
    write(STDOUT_FILENO, buf, 3);
}