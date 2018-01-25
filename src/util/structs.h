//
// Created by PHolanda on 17/12/17.
//

#ifndef MULTIDEMIONSIONALINDEXING_STRUCTS_H
#define MULTIDEMIONSIONALINDEXING_STRUCTS_H
#include <queue>
#include <climits>
#include <algorithm>

struct RangeQuery {
    int64_t *leftpredicate;
    int64_t *rightpredicate;
};

struct Column {
    int64_t *data;
};

struct Row
{
    int64_t id;
    std::vector<int64_t> data;
};

#endif //MULTIDEMIONSIONALINDEXING_STRUCTS_H
