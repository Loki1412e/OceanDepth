CC = gcc
CFLAGS = -Wall -Wextra -g -O2 -std=c99
LDFLAGS = -lm

.PHONY: all distribution rarete clean

all: distribution rarete

distribution: test_distribution
	./test_distribution.out

rarete: test_rarete
	./test_rarete.out

test_distribution: test_distribution.c
	$(CC) test_distribution.c -o test_distribution.out $(CFLAGS) $(LDFLAGS)

test_rarete: test_rarete.c
	$(CC) test_rarete.c -o test_rarete.out $(CFLAGS) $(LDFLAGS)

clean:
	rm -f test_distribution.out test_rarete.out
