CC = gcc
CFLAGS = -O2 -fopenmp -g -Wall
LDFLAGS = -fopenmp

EXEC = 	bubble.run	\
	mergesort.run	\
	odd-even.run

HEADER_FILES = $(wildcard *.h)

RAND_INIT=0
SORTED_INIT=0

ifeq ($(RAND_INIT), 1)
$(Initialization of the vector with random values)
CONFIG_FLAGS += -DRINIT
endif

ifeq ($(SORTED_INIT), 1)
$(Initialization of the vector with already sorted values)
CONFIG_FLAGS += -DSINIT
endif

all: $(EXEC)

%.run: %.o utils.o cpu_stats.o
	$(CC) $(LDFLAGS) -o $@ $^

%.o: %.c $(HEADER_FILES)
	$(CC) -c $(CONFIG_FLAGS) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXEC) *.o *~ qsort.run

.PHONY: clean
