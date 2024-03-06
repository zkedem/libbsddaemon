.POSIX:

NAME = bsddaemon
CC = cc
VERSION = 1.0.0
OUTFILE = lib$(NAME).so
PREFIX = /usr/local
LIBPATH = $(PREFIX)/lib
HEADERPATH = $(PREFIX)/include

build: $(NAME).h
	$(CC) -shared -fPIC -o $(OUTFILE) $(NAME).c

clean:
	rm -f $(OUTFILE)

install:
	cp $(OUTFILE) $(LIBPATH)
	cp $(NAME).h $(HEADERPATH)

uninstall:
	rm -f $(LIBPATH)/$(OUTFILE)
	rm -f $(HEADERPATH)/$(NAME).h
