#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <stdio.h>
#include <string.h>

#define MAXLEN     128

typedef struct msgbuf {
         long    mtype;
         char    mtext[MAXLEN];
         } message_buf;

int main(){

    message_buf cur_msg;
    int msgid = msgget(15, IPC_CREAT | 0777);

    printf("msg id = %i\n", msgid);
    cur_msg.mtype = 3;
    printf("snd = %i\n", msgsnd(msgid, (const void*) &cur_msg, MAXLEN, 0));
}