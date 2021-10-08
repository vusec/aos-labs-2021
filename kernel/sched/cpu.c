#include <cpu.h>

struct cpuinfo cpus[NCPUS];
struct cpuinfo *boot_cpu = cpus;
size_t ncpus = 1;

