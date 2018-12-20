CC = clang++

PARAMS = -O3 -Wall -Wextra -pedantic

all: objects/standard_cracking.o objects/branchless_full_scan.o objects/cracking_kd_tree.o

objects/standard_cracking.o: standard_cracking/standard_cracking.cpp standard_cracking/standard_cracking.h
	$(CC) $(PARAMS) -c standard_cracking/standard_cracking.cpp -o objects/standard_cracking.o -I /usr/local/Cellar/boost/1.68.0

objects/branchless_full_scan.o: branchless_full_scan/branchless_full_scan.cpp branchless_full_scan/branchless_full_scan.h
	$(CC) $(PARAMS) -c branchless_full_scan/branchless_full_scan.cpp -o objects/branchless_full_scan.o

objects/cracking_kd_tree.o: cracking_kd_tree/cracking_kd_tree.cpp cracking_kd_tree/cracking_kd_tree.h
	$(CC) $(PARAMS) -c cracking_kd_tree/cracking_kd_tree.cpp -o objects/cracking_kd_tree.o

clean:
	rm -rf objects

$(shell   mkdir -p objects)