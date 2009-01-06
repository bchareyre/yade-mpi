CC      = g++
CFLAGS  = -O3 -Wall

SRC = main.cpp \
      SpherePadder.cpp TetraMesh.cpp

OBJS = $(subst .cpp,.o,$(SRC))

.SUFFIXES: .cpp
.cpp.o:
	$(CC) -c $(CFLAGS) $<

all: $(OBJS)
	$(CC) -o pad $(OBJS)

clean:
	rm -f *~ \#*\#
	rm -f *.o

depend:
	makedepend -- $(CFLAGS) -- *.cpp

# DO NOT DELETE


