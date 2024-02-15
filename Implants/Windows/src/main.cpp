
#include "stdafx.h"

#include "Foocator.h"
#include "CoughLoader.h"


// Reads the object bytes from file.
unsigned char* getObject(char* objFile, size_t* fileSize)
{
	FILE* obj = fopen(objFile, "rb");
	if (obj == NULL) {
		printf("Failed to open object file! Error code: %d", GetLastError());
		return NULL;
	}
	
	fseek(obj, 0, SEEK_END);
	*fileSize = ftell(obj);
	rewind(obj);

	unsigned char* buffer = (unsigned char*)malloc(*fileSize);
	memset(buffer, '\0', *fileSize);

	if (fread(buffer, *fileSize, 1, obj) <= 0) {
		printf("Failed to read the object file!");
		return NULL;
	}

	fclose(obj);
	return buffer;
}

int main(int argc, char* argv[]) {

    printf("Initializing implant...\n");

    // if (parseObject())
    return 0;    
}