CC      = g++
CFLAGS  = -fpic -O3 -Wall
LFLAGS  = -lCGAL -lm

SRC = main.cpp \
      SpherePadder.cpp TetraMesh.cpp CellPartition.cpp

OBJS = $(subst .cpp,.o,$(SRC))

.SUFFIXES: .cpp
.cpp.o:
	$(CC) -c $(CFLAGS) $<

all: $(OBJS) 
	$(CC) -o pad $(OBJS) ./SpherePackingTriangulation/SpherePackingTriangulation.o $(LFLAGS)

pymod: $(SRC) SpherePadder_wrapper.cpp
	$(CC) SpherePadder_wrapper.cpp -shared -o packing.so $(OBJS) ./SpherePackingTriangulation/SpherePackingTriangulation.o -lboost_python -I/usr/include/python2.5 -lCGAL

clean:
	rm -f *~ \#*\#
	rm -f *.o

depend:
	makedepend -- $(CFLAGS) -- *.cpp

# DON'T FORGET TO TYPE make depend AT FIRST COMPILATION 
# DO NOT DELETE

CellPartition.o: CellPartition.hpp TetraMesh.hpp /usr/include/stdlib.h
CellPartition.o: /usr/include/features.h /usr/include/sys/cdefs.h
CellPartition.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
CellPartition.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
CellPartition.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
CellPartition.o: /usr/include/time.h /usr/include/endian.h
CellPartition.o: /usr/include/bits/endian.h /usr/include/sys/select.h
CellPartition.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
CellPartition.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
CellPartition.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
CellPartition.o: /usr/include/math.h /usr/include/bits/huge_val.h
CellPartition.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
SpherePadder.o: SpherePadder.hpp TetraMesh.hpp /usr/include/stdlib.h
SpherePadder.o: /usr/include/features.h /usr/include/sys/cdefs.h
SpherePadder.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
SpherePadder.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
SpherePadder.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
SpherePadder.o: /usr/include/time.h /usr/include/endian.h
SpherePadder.o: /usr/include/bits/endian.h /usr/include/sys/select.h
SpherePadder.o: /usr/include/bits/select.h /usr/include/bits/sigset.h
SpherePadder.o: /usr/include/bits/time.h /usr/include/sys/sysmacros.h
SpherePadder.o: /usr/include/bits/pthreadtypes.h /usr/include/alloca.h
SpherePadder.o: /usr/include/math.h /usr/include/bits/huge_val.h
SpherePadder.o: /usr/include/bits/mathdef.h /usr/include/bits/mathcalls.h
SpherePadder.o: CellPartition.hpp
TetraMesh.o: TetraMesh.hpp /usr/include/stdlib.h /usr/include/features.h
TetraMesh.o: /usr/include/sys/cdefs.h /usr/include/bits/wordsize.h
TetraMesh.o: /usr/include/gnu/stubs.h /usr/include/gnu/stubs-32.h
TetraMesh.o: /usr/include/sys/types.h /usr/include/bits/types.h
TetraMesh.o: /usr/include/bits/typesizes.h /usr/include/time.h
TetraMesh.o: /usr/include/endian.h /usr/include/bits/endian.h
TetraMesh.o: /usr/include/sys/select.h /usr/include/bits/select.h
TetraMesh.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
TetraMesh.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
TetraMesh.o: /usr/include/alloca.h /usr/include/math.h
TetraMesh.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
TetraMesh.o: /usr/include/bits/mathcalls.h
main.o: SpherePadder.hpp TetraMesh.hpp /usr/include/stdlib.h
main.o: /usr/include/features.h /usr/include/sys/cdefs.h
main.o: /usr/include/bits/wordsize.h /usr/include/gnu/stubs.h
main.o: /usr/include/gnu/stubs-32.h /usr/include/sys/types.h
main.o: /usr/include/bits/types.h /usr/include/bits/typesizes.h
main.o: /usr/include/time.h /usr/include/endian.h /usr/include/bits/endian.h
main.o: /usr/include/sys/select.h /usr/include/bits/select.h
main.o: /usr/include/bits/sigset.h /usr/include/bits/time.h
main.o: /usr/include/sys/sysmacros.h /usr/include/bits/pthreadtypes.h
main.o: /usr/include/alloca.h /usr/include/math.h
main.o: /usr/include/bits/huge_val.h /usr/include/bits/mathdef.h
main.o: /usr/include/bits/mathcalls.h CellPartition.hpp
