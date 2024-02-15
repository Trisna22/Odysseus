#       Project:		CrypterBuilder
#       Author:         ramb0
#       Version:        1.0
#		- CRYPTER 1 : In memory execute new (encrypted) executable.
#		- CRYPTER 2 : Sections encrypt functionality of CRYPTER1.
#		- TARGET    : Target project to encrypt.
#       - STUB      : The full finished binary.

CC              = g++ --no-warnings
MAIN_FILE       = main.cpp
SOURCE_FOLDER   = ./src
BUILD_FOLDER    = ./bin
CRYPTER1        = crypter_stage1
CRYPTER2		= crypter_stage2
STUB            = stub
TARGET          = target

default:
	mkdir -p ./bin/
	mkdir -p ./src/
	mkdir -p ./config/

	$(CC) $(SOURCE_FOLDER)/$(CRYPTER2)/$(CRYPTER2).cpp -o $(BUILD_FOLDER)/$(CRYPTER2)
	$(CC) $(SOURCE_FOLDER)/$(CRYPTER1)/$(CRYPTER1).cpp -o $(BUILD_FOLDER)/$(CRYPTER1)
	$(CC) $(SOURCE_FOLDER)/$(TARGET)/$(TARGET).cpp -o $(BUILD_FOLDER)/$(TARGET)
	$(CC) $(SOURCE_FOLDER)/$(STUB)/$(STUB).cpp -o $(BUILD_FOLDER)/$(STUB)

run:
	@echo =============== STAGE 1 ===============
	$(BUILD_FOLDER)/$(CRYPTER1) $(BUILD_FOLDER)/$(TARGET)

	@echo Place the encrypted binary in stub source.
	python3 $(SOURCE_FOLDER)/$(CRYPTER1)/placePayload.py

	@echo Now compile binary for stage 2.
	g++ --no-warnings -o $(BUILD_FOLDER)/$(STUB) $(SOURCE_FOLDER)/$(STUB)/$(STUB).cpp

	@echo =============== STAGE 2 ===============
	$(BUILD_FOLDER)/$(CRYPTER2) $(BUILD_FOLDER)/$(STUB)

	@echo "============= STUB ==============\n"
	
	$(BUILD_FOLDER)/$(STUB)
