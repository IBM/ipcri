CC = gcc
CFLAGS = -g -O3 -m64 -maltivec

TARGETS = chacha_test

all: $(TARGETS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $<

chacha_test:	chacha_test.o chacha-p10le-ref.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
	rm -f *.o


.PHONY: all clean

