CC = gcc

CFLAGS = -I .
LDLIBS = -lglut -lGLU -lGL -lm -lstdc++ -lopenal

perfume: perfume.o wav.o bvh.o
	$(CC) $(CFLAGS) perfume.o wav.o bvh.o -o perfume $(LDLIBS)
perfume.o: ../../src/perfume.cpp
	$(CC) $(CFLAGS) -c ../../src/perfume.cpp -o perfume.o $(LDLIBS)
wav.o: ../../src/wav.cpp
	$(CC) $(CFLAGS) -c ../../src/wav.cpp -o wav.o $(LDLIBS)
bvh.o: ../../src/bvh.cpp
	$(CC) $(CFLAGS) -c ../../src/bvh.cpp -o bvh.o $(LDLIBS)
