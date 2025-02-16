# Project: 	Windows implant
# Description:	Implant for windows hosts as part of the Odysseus framework
CC="x86_64-w64-mingw32-gcc"
PP="x86_64-w64-mingw32-g++"
OUTPUT_FOLDER="./bin"
SOURCE_BOF=./src/test_bof.c # WARNING: must be .c because symbolnames will be different.
SOURCE_IMPLANT=./src/main.cpp 
IMPLANT_CC_OPTIONS=-static-libgcc -static-libstdc++ -L /usr/x86_64-w64-mingw32/lib/ -I /usr/x86_64-w64-mingw32/include/
BEACON_INCLUDES_HEADERS=-I ../../BOFs/Windows/includes/

all: bof implant

bof: 
	@echo Compiling BOF...
	$(CC) -c $(SOURCE_BOF)  -o $(OUTPUT_FOLDER)/bof.o $(BEACON_INCLUDES_HEADERS)

implant:
	@echo Compiling implant...
	$(PP) $(BEACON_COMPATIBILITY) $(IMPLANT_CC_OPTIONS) $(BEACON_INCLUDES_HEADERS) $(SOURCE_IMPLANT) -o $(OUTPUT_FOLDER)/implant.exe

check:
	@echo Checking code...
	cppcheck -f -I /usr/x86_64-w64-mingw32/include/ ./src/