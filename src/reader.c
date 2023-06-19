#include <stdlib.h>
#include <pthread.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "reader.h"

volatile sig_atomic_t done = 0;

Read_data read_data = {.num_of_cpus = -1, .info = 0, .info_buffer = 0, .lock_writing = 1, .lock_reading = 0};

pthread_mutex_t reader_info_mutex;
pthread_cond_t cond_read;
pthread_cond_t cond_wrote;

FILE* stat_file;

int get_num_of_cpus(char* file_name){
  char* line = NULL;
  size_t len = 0;
  short count = 0;

  stat_file = fopen(file_name, "r");
  if (stat_file == NULL){
    //log error?
    return -1;
  }

  while (getline(&line, &len, stat_file) != -1) {
    if(strncmp(line, "cpu", 3) == 0)
      count++;
    else
      break;
  }
  fclose(stat_file);
  stat_file = NULL;
  free(line);
  return count;
}

void reader_init(){
  read_data.num_of_cpus = get_num_of_cpus("/proc/stat");
  read_data.info = calloc((read_data.num_of_cpus) * 8, sizeof(int));
  read_data.info_buffer = calloc((read_data.num_of_cpus) * 8, sizeof(int));
  read_data.lock_writing = 0;
  read_data.lock_reading = 0;
}

void reader_loop(){
  char* line = NULL;
  size_t len = 0;
  short r = 0;

  stat_file = fopen("/proc/stat", "r");
  if (stat_file == NULL){
    //log error?
    return;
  }

  while (getline(&line, &len, stat_file) != -1) {
    if(strncmp(line, "cpu", 3) == 0){
      sscanf(line, "%*s %d %d %d %d %d %d %d %d", 
        &read_data.info_buffer[r * 8 + USER],
        &read_data.info_buffer[r * 8 + NICE],
        &read_data.info_buffer[r * 8 + SYSTEM],
        &read_data.info_buffer[r * 8 + IDLE],
        &read_data.info_buffer[r * 8 + IOWAIT],
        &read_data.info_buffer[r * 8 + IRQ],
        &read_data.info_buffer[r * 8 + SOFTIRQ],
        &read_data.info_buffer[r * 8 + STEAL]
      );
      r++;
    }
    else
      break;
  }
  fclose(stat_file);
  stat_file = NULL;
  free(line);

  pthread_mutex_lock(&reader_info_mutex);
  while (read_data.lock_reading)
    pthread_cond_wait(&cond_read, &reader_info_mutex);
  read_data.lock_writing = 1;

  int* temp = read_data.info_buffer;
  read_data.info_buffer = read_data.info;
  read_data.info = temp;

  read_data.lock_writing = 0;
  pthread_cond_signal(&cond_wrote);
  pthread_mutex_unlock(&reader_info_mutex);
}

void reader_destroy(){
  free(read_data.info);
  free(read_data.info_buffer);
  if(stat_file != NULL)
    fclose(stat_file);
}

void* reader_main(void* args){
  reader_init();
  int a = 0;
  while(!done){
    reader_loop();
    usleep(500000);
  }
  reader_destroy();
  pthread_exit(0);
}