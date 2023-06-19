#ifndef READER_H
#define READER_H

#include <stdatomic.h>
#include <stdbool.h>
#include <pthread.h>
#include <signal.h>

extern pthread_mutex_t reader_info_mutex;
extern pthread_cond_t cond_read;
extern pthread_cond_t cond_wrote;

extern volatile sig_atomic_t done;

enum {
    USER, 
    NICE,
    SYSTEM,
    IDLE,
    IOWAIT,
    IRQ,
    SOFTIRQ,
    STEAL
};

typedef struct
{
    short num_of_cpus;

    int* info;
    int* info_buffer;

    bool lock_writing;
    bool lock_reading;
} Read_data;

extern Read_data read_data; 

int get_num_of_cpus();
void reader_init();
void reader_loop();
void reader_destroy();
void* reader_main(void* args);

#endif