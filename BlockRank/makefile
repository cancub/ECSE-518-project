CC=gcc
CFLAGS= -g -Wall

default: blockrank

# To create the executable file count we need the object files
# BlockRank.o, dubarrays.o, and dubvmalg.o:
#
blockrank:  BlockRank.o dubarrays.o dubvmalg.o twodarrays.o
	$(CC) $(CFLAGS) -o blockrank BlockRank.o dubarrays.o dubvmalg.o twodarrays.o -lm -lcblas

# To create the object file BlockRank.o, we need the source
# files BlockRank.c, dubvmalg.h, and dubarrays.h:
#
BlockRank.o:  BlockRank.c dubvmalg.h dubarrays.h twodarrays.h
	$(CC) $(CFLAGS) -c BlockRank.c

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
	$(RM) blockrank *.o *~


# all: BlockRank.c
# 	gcc -g -Wall -o blockrank BlockRank.c dubarrays.c dubvmalg.c twodarrays.c -lm -lcblas

# clean:
# 	$(RM) blockrank