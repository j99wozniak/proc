#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include "analyzer.h"
#include "reader.h"

double* cpu_percentages = 0;
short num_of_cpus = 0;
int* prev_total;
int* prev_idle;

void analyzer_init(){
  pthread_mutex_lock(&reader_info_mutex);
  while (read_data.lock_writing)
    pthread_cond_wait(&cond_wrote, &reader_info_mutex);
  read_data.lock_reading = 1;

  num_of_cpus = read_data.num_of_cpus;
  cpu_percentages = calloc(num_of_cpus, sizeof(double));
  prev_total = calloc(num_of_cpus, sizeof(int));
  prev_idle = calloc(num_of_cpus, sizeof(int));

  read_data.lock_reading = 0;
  pthread_cond_signal(&cond_read);
  pthread_mutex_unlock(&reader_info_mutex);
}

double calculate_percentage(int Idle, int Prev_idle, int Total, int Prev_total){
  if(Total-Prev_total==0){
    return 0.0;
  }
  return (100.0 - ((double)(Idle-Prev_idle)/(double)(Total-Prev_total))*100.0);
}

void analyze(){
  pthread_mutex_lock(&reader_info_mutex);
  while (read_data.lock_writing)
    pthread_cond_wait(&cond_wrote, &reader_info_mutex);
  read_data.lock_reading = 1;
  
  int i;
  for(i = 0; i<num_of_cpus; i++){
    int Idle = read_data.info[i*8+IDLE];
    int NonIdle = read_data.info[i*8+USER] + 
                  read_data.info[i*8+NICE] + 
                  read_data.info[i*8+SYSTEM] +
                  read_data.info[i*8+IRQ] +
                  read_data.info[i*8+SOFTIRQ] +
                  read_data.info[i*8+STEAL];
    int Total = Idle + NonIdle;
    cpu_percentages[i] = calculate_percentage(Idle, prev_idle[i], Total, prev_total[i]);
    prev_total[i] = Total;
    prev_idle[i] = Idle;
  }

  read_data.lock_reading = 0;
  pthread_cond_signal(&cond_read);
  pthread_mutex_unlock(&reader_info_mutex);
}

void analyzer_destroy(){
  free(cpu_percentages);
  free(prev_total);
  free(prev_idle);
}

void* analyzer_main(void* args){
  analyzer_init();
  while(!done){
    analyze();
    usleep(500000);
  }
  analyzer_destroy();
  pthread_exit(0);
}