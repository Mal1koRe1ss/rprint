#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include "rprint.h"

uint64_t get_nanosec() {
	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC, &ts);
	return (uint64_t)ts.tv_sec * 1000000000ULL + (uint64_t)ts.tv_nsec; 
}

int main(void){
	const int iterations = 100000;
	uint64_t start, end;
	double rprint_time, std_time;

	printf("Performance comparison: Custom printf VS Standard printf\n");
	printf("Testing with %d iterations each...\n\n", iterations);

	start = get_nanosec();
	for (int i = 0; i < iterations; i++) { printf("Test %d: %s %%\n", i, "Hello World"); }
	end = get_nanosec();
	std_time = (double)(end - start) / 1000000000.0;

	start = get_nanosec();
	for (int i = 0;i < iterations; i++) { rprint("Test %d: %s %%\n", i, "Hello World"); }
	end = get_nanosec();
	rprint_time = (double)(end - start) / 1000000000.0;

	printf("\nResults:\n");
	printf("Standard printf: %.6f seconds (%.2f ns/call)\n", std_time, (std_time * 1000000000.0) / iterations);
	printf("Custom printf: %.6f seconds (%.2f ns/call)\n", rprint_time, (rprint_time * 1000000000.0) / iterations);

} 
