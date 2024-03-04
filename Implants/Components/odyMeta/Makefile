CC="g++"

SEED := $(shell /bin/bash -c "echo $$(shuf -i 1-999999 -n 1)")

all:
	g++ ./src/main.cpp -o ./bin/meta -D RANDOMINT=$(SEED)

run:
	./bin/meta