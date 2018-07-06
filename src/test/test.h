#ifndef MULTIDEMIONSIONALINDEXING_TEST_H
#define MULTIDEMIONSIONALINDEXING_TEST_H
#include "../util/structs.h"
#include <unordered_map>
#include <vector>
#include "../util/util.h"
#include "../cracking/standard_cracking.h"
#include "../cracking/kd_tree.h"
#include "../cracking/sideways/sideways_cracking.h"
#include "../cracking/sideways/partial_sideways_cracking.h"

using namespace std;
void verifyAlgorithms(Table *table, vector<vector<pair<int64_t,int64_t>>> rangeQueries);
#endif //MULTIDEMIONSIONALINDEXING_TEST_H
