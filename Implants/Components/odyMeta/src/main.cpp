#include <iostream>
#include <stdio.h>
#include <unistd.h>

// Check if compiler gives us an random value for metamorphism.
#ifndef RANDOMINT
    #define RANDOMINT 0x12345678
#endif

/**
 * Inserting garbage code based on random value.
 */
#if RANDOMINT % 100 < 33
	void garbageCode() {
		printf("This is garbage code 1\n");
	}
#elif RANDOMINT % 100 > 66
	void garbageCode() {
		printf("This is garbage code 2\n");
	}
#else
	void garbageCode() {
		printf("This is garbage code 3\n");
	}
#endif 

/**
 *  Randomized code substitution 
 */
#ifdef RANDOMINT % 2 == 0
	#define CODE_SUBSTITUTION
#endif

void importantFunction() {

	#ifdef CODE_SUBSTITUTION 
		printf("fopen()\n");
		FILE* reader = fopen("virus.exe", "rb");
	#else
		printf("open()\n");
		int fd = open("virus.exe", O_RDONLY);
	#endif
	
	printf("Very important function!\n");	
	printf("Executing BOF...\n");

	char buffer[1000];
	#ifdef CODE_SUBSTITUTION
		fread(buffer, 1000, 1, reader);
	#else
		read(fd, buffer, 1000);
	#endif 
	
} 

int main(int argc, char* argv[])
{
	printf("Starting metamorphism kit...\n\n");

	printf("Random number: %d\n", RANDOMINT);
	garbageCode();

	importantFunction();

	return 0;
}
