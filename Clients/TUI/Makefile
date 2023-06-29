# Project:       Odysseus CLI (odyCLI)
# Author:        ramb0

CC              = "g++"
SOURCE_FOLDER   = "./src/"
BUILD_FOLDER    = "./bin"
MAIN_FILE       = "main.cpp"
PROJECT         = "odyCLI"

default:
	mkdir -p $(BUILD_FOLDER)
	$(CC) -o $(BUILD_FOLDER)/$(PROJECT) $(SOURCE_FOLDER)/$(MAIN_FILE)

run:
	$(BUILD_FOLDER)/$(PROJECT)
