CC     = gcc
CFLAGS = -Wno-incompatible-pointer-types
SRCS   = $(wildcard src/*.c)

all: a.out

a.out: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o a.out

clean:
	rm -f a.out
