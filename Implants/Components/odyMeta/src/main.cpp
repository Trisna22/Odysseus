#include <iostream>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

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
#if RANDOMINT % 2 == 0
	#define CODE_SUBSTITUTION 1
#else 
	#define CODE_SUBSTITUTION 0
#endif

void importantFunction() {
	
	printf("Very important function!\n");	
	
	#if CODE_SUBSTITUTION 
		printf("fopen()\n");
	#else
		printf("open()\n");
	#endif

	printf("Loading BOF...\n");	
} 

int main(int argc, char* argv[])
{
	printf("Starting metamorphism kit...\n\n");

	printf("Random number: %d\n", RANDOMINT);
	garbageCode();

	importantFunction();

	return 0;
}
