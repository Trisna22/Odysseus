
#include "stdafx.h"
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

	printf("Initializing BOF debugger...\n");

	if (argc != 2) {
		printf("%s [bof-file]\n", argv[0]);
		return 1;
	}

	size_t objSize;
	unsigned char* objContent = getObject(argv[1], &objSize);
	if (objContent == NULL) {
		return 1;
	}

	parseObject(objContent, objSize);

    return 0;    
}

