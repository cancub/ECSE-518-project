CC=gcc
CFLAGS= -g -Wall

default: pagerank

# To create the executable file count we need the object files
# PageRank.o, dubarrays.o, and dubvmalg.o:
#
pagerank:  PageRank.o dubarrays.o dubvmalg.o twodarrays.o
	$(CC) $(CFLAGS) -o pagerank PageRank.o dubarrays.o dubvmalg.o twodarrays.o -lm -lcblas

# To create the object file PageRank.o, we need the source
# files PageRank.c, dubvmalg.h, and dubarrays.h:
#
PageRank.o:  PageRank.c dubvmalg.h dubarrays.h twodarrays.h
	$(CC) $(CFLAGS) -c PageRank.c

# To create the object file dubarrays.o, we need the source files
# dubarrays.c and dubarrays.h:
#
dubarrays.o:  dubarrays.c dubarrays.h 
	$(CC) $(CFLAGS) -c dubarrays.c

# To create the object file dubvmalg.o, we need the source files
# dubvmalg.c and dubvmalg.h:
#
dubvmalg.o:  dubvmalg.c dubvmalg.h 
	$(CC) $(CFLAGS) -c dubvmalg.c

twodarrays.o:  twodarrays.c twodarrays.h 
	$(CC) $(CFLAGS) -c twodarrays.c

# To start over from scratch, type 'make clean'.  This
# removes the executable file, as well as old .o object
# files and *~ backup files:
#
clean: 
	$(RM) pagerank *.o *~


# all: PageRank.c
# 	gcc -g -Wall -o pagerank PageRank.c dubarrays.c dubvmalg.c twodarrays.c -lm -lcblas

# clean:
# 	$(RM) pagerank