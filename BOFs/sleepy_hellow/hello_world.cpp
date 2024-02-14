#include <iostream>
#include <unistd.h>

#define SLEEP_SECONDS COUNT_SLEEP

int payload_init() {
	printf("Hello world!\n");

	for (int i = 0; i < SLEEP_SECONDS; i++) {
		printf("Sleeping for %d seconds yet...\n", SLEEP_SECONDS - i);
		sleep(1);
	}
	return 22;
}
