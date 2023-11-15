CC = gcc
CFLAGS = -Wall -Wextra -std=c99

all: psearch1a psearch1b psearch1c

psearch1a: psearch1a.c
	$(CC) $(CFLAGS) psearch1a.c -o psearch1a

psearch1b: psearch1b.c
	$(CC) $(CFLAGS) psearch1b.c -o psearch1b

psearch1c: psearch1c.c
	$(CC) $(CFLAGS) psearch1c.c -o psearch1c

clean:
	rm -f psearch1a psearch1b psearch1c
