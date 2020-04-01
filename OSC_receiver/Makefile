CC= gcc

LIBS= -llo  -lwiringPi

OSC_simple_client: OSC_simple_client.c
	$(CC) OSC_simple_client.c -Werror $(LIBS) -std=gnu99 -O2 -g -o OSC_simple_client

OSC_serveur_client: main_client_server.c
	$(CC) main_client_server.c -Werror $(LIBS) -std=gnu99 -O2 -g -o OSC_client_server

all: OSC_simple_client OSC_serveur_client

.PHONY: clean

clean:
	rm -f OSC_simple_client OSC_server_client
