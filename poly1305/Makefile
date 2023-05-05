CC = gcc
CFLAGS = -g -O3 -m64 -mcpu=power9 -maltivec

TARGETS = poly1305_test

all: $(TARGETS)

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.S
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.s
	$(CC) $(CFLAGS) -c -o $@ $<

poly1305_test:	poly1305_test.o poly1305-p10le-ref.o poly1305-p10le-44-ref.o
	$(CC) $(CFLAGS) -o $@ $^

clean:
	rm -f $(TARGETS)
	rm -f *.o


.PHONY: all clean

