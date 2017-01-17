/* test C Language api */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <mcheck.h>
#include <signal.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/prctl.h>

void *unfree_thread(void *data)
{
    char *unfree;
    int i;

    pthread_detach(pthread_self());
    prctl(PR_SET_NAME, "unfree_thr");
    i = 10000;
    while (i--) {
        unfree = (char *)malloc(15);
        bzero(unfree, 15);
        strcpy(unfree, "thread unfree.");
    }

    while (1) {
        unfree = malloc(10);
        sleep(1999);
    }

    return NULL;
}

void signal_handler(int sig)
{
    muntrace();
    exit(0);
}

int main(int argc, char *argv[])
{
    char *unfree;
    pthread_t tid;
    int i, ret;

    mtrace();

    signal(SIGINT, signal_handler);

    ret = pthread_create(&tid, NULL, unfree_thread, NULL);
    if (ret != 0) {
        printf("unfree thread create fail.");
        return -1;
    }


    for (i = 0; i < 100; i++) {
        unfree = (char *)calloc(1, 10);
        strcpy(unfree, "unfree");
    }

    pthread_join(tid, NULL);

    muntrace();


    return 0;
}
