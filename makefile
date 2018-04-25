all: build

build:
	gcc -g -o client client.c
	gcc -g -o server server.c

clean:
	rm server
	rm client
