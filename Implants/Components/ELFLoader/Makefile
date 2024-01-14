# Project Name: 	ELFLoader
# Author:		ramb0
# Description:		BOF loader for ELF binaries on Linux.

CC="g++"
SOURCES_LOADER="./src/loader/"
SOURCES_BOF="./src/bof/"
OUTPUT="./bin/"

all: loader bof

loader:
	$(CC) $(SOURCES_LOADER)/main.cpp -o $(OUTPUT)/loader

bof:
	$(CC) -c $(SOURCES_BOF)/bof.cpp -o $(OUTPUT)/bof.so
