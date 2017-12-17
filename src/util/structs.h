//
// Created by PHolanda on 17/12/17.
//

#ifndef MULTIDEMIONSIONALINDEXING_STRUCTS_H
#define MULTIDEMIONSIONALINDEXING_STRUCTS_H
#include <queue>
#include <climits>
#include <algorithm>

struct RangeQuery {
    std::vector<int64_t> leftpredicate;
    std::vector<int64_t> rightpredicate;
};

struct Column {
    std::vector<int64_t> data;
};

#endif //MULTIDEMIONSIONALINDEXING_STRUCTS_H
