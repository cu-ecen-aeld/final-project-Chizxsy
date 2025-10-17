#ifndef USBLIB_H
#define USBLIB_H

#include <pthread.h>

typedef struct {
    pthread_mutex_t mutex;
    char buffer[2048];
    int data_ready_flag;
} usblib_t;

#endif