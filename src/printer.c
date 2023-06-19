#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "analyzer.h"
#include "reader.h"


void* printer_main(void* args){
  sleep(1);
  while(!done){
    printf("CPU:%03.2f%%", cpu_percentages[0]);
    int r;
    for(r = 1; r<num_of_cpus; r++){
      printf("|CPU%d:%03.2f%%", r-1, cpu_percentages[r]);
    }
    printf("\n");
    sleep(1);
  }
  pthread_exit(0);
}