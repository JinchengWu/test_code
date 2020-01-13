/* test C Language api */

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/prctl.h>
#include <sys/syscall.h>

static pthread_rwlock_t g_rw_lck = PTHREAD_RWLOCK_INITIALIZER;
static int g_mem;

static void *read_handle(void *data)
{
    pid_t tid;

    tid = syscall(SYS_gettid);
    prctl(PR_SET_NAME, __func__);
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        printf("%d %s, try get mem rw_lck\r\n", tid, __func__);
        pthread_rwlock_rdlock(&g_rw_lck);
        printf("%s, get rw_lck, read mem: %d\n", __func__, g_mem);
        pthread_rwlock_unlock(&g_rw_lck);
    }

    return NULL;
}

static void *write_handle(void *data)
{
    prctl(PR_SET_NAME, __func__);
    pthread_detach(pthread_self());
    while (1) {
        sleep(1);
        printf("%s, get rw_lck mutex\r\n", __func__);
        pthread_rwlock_wrlock(&g_rw_lck);
        sleep(3);
        g_mem++;
        printf("%s, write free rw_lck mutex\r\n", __func__);
        pthread_rwlock_unlock(&g_rw_lck);
    }

    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t tid1, tid2, tid3;
    int ret;

    ret = pthread_rwlock_init(&g_rw_lck, NULL);
    if (ret != 0) {
        printf("rwlock init\r\n");
        return -1;
    }
    ret = pthread_create(&tid1, NULL, read_handle, NULL);
    if (ret != 0) {
        printf("pthread create fail\r\n");
        return -1;
    }
    ret = pthread_create(&tid2, NULL, read_handle, NULL);
    if (ret != 0) {
        printf("pthread create fail\r\n");
        return -1;
    }
    ret = pthread_create(&tid3, NULL, write_handle, NULL);
    if (ret != 0) {
        printf("pthread create fail\r\n");
        return -1;
    }
    while (1) {
        sleep(1);
    }

    return -1;
}
