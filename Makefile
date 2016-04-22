CC=cc
CFLAGS=-Wall
SOURCES=main.c
EXECUTABLE=beep
INSTALLDIR=/usr/local/bin

all:
	$(CC) $(CFLAGS) $(SOURCES) -o $(EXECUTABLE)
clean: 
	rm $(EXECUTABLE)
install: 
	install $(EXECUTABLE) $(INSTALLDIR)
uninstall: 
	rm $(INSTALLDIR)/$(EXECUTABLE)
