.POSIX:

NAME = bsddaemon
OUTFILE = lib$(NAME).so
INSTALLPATH = /usr/local/lib

build: $(NAME).h
	cc -shared -fPIC -o $(OUTFILE) $(NAME).c

clean:
	rm -f $(OUTFILE)

install:
	install $(OUTFILE) $(INSTALLPATH)

uninstall:
	rm -f $(INSTALLPATH)/$(OUTFILE)
