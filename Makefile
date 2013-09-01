CC = gcc

CFLAGS = -pipe -Wall -Wextra -std=c99 -pedantic-errors -Werror -O3 -DRELAY_USE_DISPATCHER
CFLAGS = -pipe -Wall -Wextra -std=c99 -pedantic -g -DRELAY_USE_DISPATCHER


#bundled iniparser
INIPARSER = -Ldeps/iniparser/ -liniparser

LINKERFLAGS = $(INIPARSER) -lpthread -fwhole-program


LINT = splint
LINTFLAGS = +quiet +weak -warnposix


TARGET = RIDRelay
INSTALLATIONDIR = /usr/local/bin
.SUFFIXES:

OBJS = program.o \
		Diagnostic/Log.o \
		Util/Path.o Util/queue.o Util/LinkedList.o \
		network/Packet.o network/Tunnel.o network/accept.o \
	
		

LIBS = deps/iniparser/libiniparser.a \


.PHONY : all
all : $(TARGET)

.PHONY : install
install : $(TARGET)
	cp $(TARGET) $(INSTALLATIONDIR)

.PHONY : clean
clean :
	-rm $(OBJS)
	-cd deps/iniparser && make veryclean
	-find . -type f -name "*.h.gch" -exec rm -f {} \;

$(TARGET) : $(OBJS) $(LIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LINKERFLAGS) -o $(TARGET)

deps/iniparser/libiniparser.a : deps/iniparser/Makefile
	cd deps/iniparser && make default

%.o : %.c %.h.gch
	-$(LINT) $(LINTFLAGS) $<
	$(CC) $(CFLAGS) -o $@ -c $<

%.h.gch : %.h
	$(CC) $(CFLAGS) -o $@ -c $^
