# Project:	Odysseus
# Title: 	Linux implant of Odysseus C2
# Author:	ramb0
CC 		= g++ 
SOURCE	= ./src
OUTPUT	= ./bin
MAIN	= main.cpp
PROJECT = implant

default:
	mkdir -p ./bin
	if [[ -z IMPLANT_ID ]]; then
		$(CC) -o $(OUTPUT)/$(PROJECT) $(SOURCE)/$(MAIN);
	else
		$(CC) -D SLAVE_ID=\"$(IMPLANT_ID)\" -o $(OUTPUT)/$(PROJECT) $(SOURCE)/$(MAIN);
	fi

run:
	$(OUTPUT)/$(PROJECT)
