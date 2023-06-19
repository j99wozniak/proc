#ifndef ANALYZER_H
#define ANALYZER_H

void analyzer_init();
double calculate_percentage(int Idle, int Prev_idle, int Total, int Prev_total);
void analyze();
void analyzer_destroy();
void* analyzer_main(void* args);

extern double* cpu_percentages;
extern short num_of_cpus;

#endif