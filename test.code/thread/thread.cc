#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <map>
#include <vector>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <strings.h>
#include <pthread.h>
#include <map>
#include <vector>
using namespace std;

void *threadFunc ( void * args )
{
    /* 一些可能让线程死锁的代码 */
    while(1){
        printf("running\n");
    }
    return NULL;
}

int main (void)
{
    pthread_t mythread;
    if ( pthread_create(&mythread, NULL, threadFunc, NULL ) )
    {
        printf ( "Error Creating Thread.\n" );
        return 1;
    }
    pthread_cancle(mythread);
    pthread_join(mythread,NULL);
    sleep(3);
    /* 强制结束线程mythread，代码怎么写？  */
    return 0;
}
