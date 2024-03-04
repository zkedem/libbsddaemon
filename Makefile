.POSIX:

NAME = bsddaemon
OUTFILE = lib$(NAME).so
PREFIX = /usr/local
LIBPATH = $(PREFIX)/lib
HEADERPATH = $(PREFIX)/include

build: $(NAME).h
	cc -shared -fPIC -o $(OUTFILE) $(NAME).c

clean:
	rm -f $(OUTFILE)

install:
	cp $(OUTFILE) $(LIBPATH)
	cp $(NAME).h $(HEADERPATH)

uninstall:
	rm -f $(LIBPATH)/$(OUTFILE)
	rm -f $(HEADERPATH)/$(NAME).h
