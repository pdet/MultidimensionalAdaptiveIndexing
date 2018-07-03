

OPTIMIZE=$(OPT)

CCPP=clang++
STDFLAGS=-std=c++11

INC=-I/src/cracking


ifneq ($(OPTIMIZE), true)
	OPTFLAGS=-O0 -g
else
	OPTFLAGS=-O3 -march=native -fno-tree-vectorize
endif

adaptive_lala: src/main.cpp
	$(CCPP) $(STDFLAGS) -I. $(OPTFLAGS) src/gendata.cpp -o gendata
	$(CCPP) $(STDFLAGS) -I. $(OPTFLAGS) src/util/file_manager.cpp src/util/util.cpp src/cracking/avl_tree.cpp src/cracking/kd_tree.cpp src/cracking/standard_cracking.cpp src/cracking/sideways_cracking.cpp src/test/test.cpp src/main.cpp -o crackingmain
