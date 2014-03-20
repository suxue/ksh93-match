CC=gcc -std=gnu99
CPPFLAGS=-I/opt/uwin/include/ast
CFLAGS=-Wall -fPIC
LDFLAGS=-L/opt/uwin/lib
LIBS=-lshell -last -lpcre

test: libmatch.so
	@m4 test.sh | ksh -

libmatch.so: match.o
	$(CC) -shared -Wl,-soname,$@ $< -o $@ $(LDFLAGS) $(LIBS)

match.o: match.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $<

clean:
	rm -f match.o libmatch.so *.o

install: libmatch.so
	@install -v -D $< $$HOME/.local/lib/ksh/$<

.PHONY: test install clean
