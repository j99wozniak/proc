#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include "../src/reader.h"

int test() {
    int cpus = get_num_of_cpus("../tests/dummy_cpus");
    assert(cpus == 9);
}

int main() {
    test();
    return 0;
}