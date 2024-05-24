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
 * Garbage code using random value.
*/
void garbageCode2() {

	#if (RANDOMINT % 3) == 0

		printf("G2 0\n");

		int count = 0;
		for (int x = 0; x < (RANDOMINT % 100); x++) {
			count += x;
		}

	#elif (RANDOMINT % 2) == 0
		printf("G2 1\n");

		int v = 42;
		if ((v >> 4) < 100) {
			v += 20 << 4;
		} else if (v + (2 >> 3) == 214) {
			v -= 241;
		}
		else {
			v = v >> 12 - 14;
		}
		
	#else
		printf("G2 2\n");
		int j = RANDOMINT % 100;
		for (int i = j; i > 0; i--) {
			j = j /2;
		}
	#endif
}

/**
 * Randomized code substitution 
 */
#if (RANDOMINT % 2) == 0
	#define CODE_SUBSTITUTION 1
#else 
	#define CODE_SUBSTITUTION 0
#endif

/**
 * Indirect jumps
*/
void doSomething(int id) {

	printf("== doSomething() ===>\n");

	printf("ID: %d\n", id);

	printf("<= doSomething() ====\n");
}
void (*functionPointer)(int) = &doSomething;

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


	garbageCode(); // Insert garbage function.
	garbageCode2();

	importantFunction(); // Code substitition.

	doSomething(0); // Indirect jump (normal)
	functionPointer(1); // Indirect jump (indirect)

	return 0;
}