CC	= gcc
LINKER	= gcc
LIBS	= `sdl-config --libs` -lm
CFL_W	= -Wno-sign-compare -Wpointer-arith -Wno-system-headers -Wshadow -Wbad-function-cast -Werror -Wunreachable-code
CFLAGS	= -ansi -pedantic -pedantic-errors -Wall $(CFLAGS_W) `sdl-config --cflags`
LDFLAGS	= -s
OBJ	= errors.o morse.o audio.o main.o
BINARY	= morse-x
CLEAN	= *~ *.bak *.o $(BINARY) Makefile.headers

.SUFFIXES: .c .o

all: $(OBJ)
	$(LINKER) $(LDFLAGS) -o "$(BINARY)"	$(OBJ) $(LIBS)

include Makefile.headers

# strange, but nice ;) taken from IMine
Makefile.headers:
	grep '^#include *".*"' *.c | sed 's/\(.*\).c:#include *"\(.*\)".*/\1.o: \2/' > Makefile.headers

%o: %c
	$(CC) $(CFLAGS) -c -o $@	$<

clean:
	rm -f $(CLEAN)

install:
	strip "$(BINARY)"
	install -d $(DESTDIR)/usr/bin
	install $(BINARY) $(DESTDIR)/usr/bin

deb:
	dpkg-buildpackage -rfakeroot

deb-clean:
	rm -rf debian/morse-x debian/files debian/*substvars
	$(MAKE) clean
