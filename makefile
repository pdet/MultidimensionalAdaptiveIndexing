

OPTIMIZE=$(OPT)

CCPP=clang++
STDFLAGS=-std=c++11

INC=-I/src/cracking


ifneq ($(OPTIMIZE), true)
	OPTFLAGS=-O0 -g
else
	OPTFLAGS=-O3 -march=native
endif

adaptive_lala: src/main.cpp
	$(CCPP) $(STDFLAGS) -I. $(OPTFLAGS) src/gendata.cpp -o gendata
	$(CCPP) $(STDFLAGS) -I. $(OPTFLAGS) src/util/file_manager.cpp src/cracking/avl_tree.cpp src/cracking/kd_tree.cpp src/cracking/standard_cracking.cpp src/fullindex/binary_search.cpp src/fullindex/bulkloading_bp_tree.cpp src/fullindex/hybrid_radix_insert_sort.cpp  src/main.cpp -o crackingmain
