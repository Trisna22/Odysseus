#include <iostream>
#include <unistd.h>

int payload_init() {
	printf("Hello world!\n");

	for (int i = 0; i < 10; i++) {
		printf("Sleeping for %d seconds yet...\n", 10 - i);
		sleep(1);
	}
	return 22;
}
