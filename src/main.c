#include <string.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include "reader.h"
#include "analyzer.h"
#include "printer.h"

void terminate(){
  done = 1;
}

int main() {

  struct sigaction action;
  memset(&action, 0, sizeof(struct sigaction));
  action.sa_handler = terminate;
  sigaction(SIGINT, &action, NULL);
  sigaction(SIGTERM, &action, NULL);

  pthread_t readerThread, analyzerThread, printerThread;

  pthread_mutex_init(&reader_info_mutex, NULL);
  pthread_cond_init(&cond_read, NULL);
  pthread_cond_init(&cond_wrote, NULL);

  pthread_create(&readerThread, NULL, reader_main, NULL);
  pthread_create(&analyzerThread, NULL, analyzer_main, NULL);
  pthread_create(&printerThread, NULL, printer_main, NULL);

  pthread_join(readerThread, NULL);
  pthread_join(analyzerThread, NULL);
  pthread_join(printerThread, NULL);

  pthread_mutex_destroy(&reader_info_mutex);
  pthread_cond_destroy(&cond_read);
  pthread_cond_destroy(&cond_wrote);

  return 0;
}