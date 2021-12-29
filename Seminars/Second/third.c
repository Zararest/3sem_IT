#include "stdio.h"
#include "stdlib.h"
#include "errno.h"
#include <unistd.h>
#include <pthread.h>

long long sum;

void* start_func(void* tmp){

    for (int i = 0; i < 10000; i++){

        sum += 1;
    }
}

int main(){

    pthread_t tid;

    sum = 0;

    for (int i = 0; i < 200; i++){

        printf("created thread[%d] = %d\n", i,  pthread_create(&tid, NULL, start_func, NULL));
    }
    
    printf("sum = %lld\n", sum);
}