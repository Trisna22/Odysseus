# C2 Server

## Implant objects
The server can take objects from clients to execute on the implants.
### Implant objects format
The implant objects are written in a specific format and has a few requirements.  
The object sources needs to be written in C++ with the following function implemented.
```C
extern "C" int payload_init() {
    // Add your code here...
    printf("Hellow evil people!\n");
}
```
### Implant objects variables
The implant objects have the possibility to take variables to make the objects dynamically for each implant. The objects will be recompiled for each implant with the variables set that the user gives. To write an object that has this possibility, the object can be written like so. (In this example TEXT is our variable for the implants) Be aware that the variable must be unique and not match other variables in the code, otherwise the object won't compile correctly.
```C
#include <iostream>
#define TO_ECHO TEXT // The variable that makes the object dynamic.

extern "C" int payload_init() {

	printf("Hello world! Message: %s\n", TO_ECHO);
	return 22;
}
```