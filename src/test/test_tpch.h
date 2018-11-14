#ifndef MULTIDEMIONSIONALINDEXING_TEST_TPCH
#define MULTIDEMIONSIONALINDEXING_TEST_TPCH
#include "../util/structs.h"
#include <unordered_map>
#include <vector>
#include "../util/util.h"
#include "../cracking/standard_cracking.h"
#include "../cracking/kd_tree.h"
#include "../cracking/sideways/sideways_cracking.h"
#include "../cracking/sideways/partial_sideways_cracking.h"
#include "../quasii/quasii.h"
#include <array>

using namespace std;
void verifyAlgorithms(Table *table, vector<vector<array<int64_t, 3>>> rangeQueries);
#endif //MULTIDEMIONSIONALINDEXING_TEST_TPCH
