# GNU Make - Makefile
# for morse-x (C) Stephan Beyer, 2004

CC	= gcc
LINKER	= gcc
CFLAGS	= -ansi -pedantic -pedantic-errors -Wall -Werror -Wunreachable-code
LDFLAGS	= -L/usr/X11R6/lib -L/usr/lib/X11R6/ -lX11 -lXpm
OBJ	= errors.o calibrate.o main.o
BINARY	= morse-x
CLEAN	= *~ *.bak *.o $(BINARY) Makefile.headers

.SUFFIXES: .c .o

all: $(OBJ)
	$(LINKER) $(LDFLAGS) -o "$(BINARY)"	$(OBJ)

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
	rm -rf debian/morse-x
	$(MAKE) clean
