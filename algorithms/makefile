CC = clang++

PARAMS = -O3 -Wall -Wextra -pedantic

all: objects/standard_cracking.o objects/branchless_full_scan.o

objects/standard_cracking.o: standard_cracking/standard_cracking.cpp standard_cracking/standard_cracking.h
	$(CC) $(PARAMS) -c standard_cracking/standard_cracking.cpp -o objects/standard_cracking.o

objects/branchless_full_scan.o: branchless_full_scan/branchless_full_scan.cpp branchless_full_scan/branchless_full_scan.h
	$(CC) $(PARAMS) -c branchless_full_scan/branchless_full_scan.cpp -o objects/branchless_full_scan.o

clean:
	rm -rf objects

$(shell   mkdir -p objects)