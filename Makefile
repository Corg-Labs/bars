CC     = gcc
CFLAGS = -O2 -Wall

bars: bars.c
	$(CC) $(CFLAGS) bars.c -o bars

run: bars
	./bars

clean:
	rm -f bars

.PHONY: run clean
