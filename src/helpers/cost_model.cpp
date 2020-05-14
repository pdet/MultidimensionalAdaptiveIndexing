
#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "cost_model.hpp"

using namespace std;
using namespace chrono;

double CostModel::write_sequential_page_cost() {
    //! initial write
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        int matching = base_column->data[i] >= low &&base_column->data[i] <= high;
        sum += matching *base_column->data[i];

        int bigger_pivot = base_column->data[i] >= node->key;
        int smaller_pivot = 1 - bigger_pivot;

        index->data[node->current_start] = base_column->data[i];
        index->data[node->current_end]  = base_column->data[i];

        node->current_start += smaller_pivot;
        node->current_end -= bigger_pivot;
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
   return  duration<double, std::milli>(end - start).count()/PAGES_TO_WRITE;
}

double CostModel::read_sequential_without_matches_page_cost() {
    //! reading
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        sum +=  index->data[i];
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return  duration<double, std::milli>(end - start).count()/PAGES_TO_WRITE;
}

double CostModel::read_sequential_with_matches_page_cost() {
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        int matching = index->data[i] >= low && index->data[i] <= high;
        sum += index->data[i]* matching;
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return  duration<double, std::milli>(end - start).count()/PAGES_TO_WRITE;
}

double CostModel::read_random_access() {
    vector<int> random_lookups;
    for (size_t i = 0; i < PAGES_TO_WRITE; i++) {
        random_lookups.push_back(rand() % PAGES_TO_WRITE);
    }

    //! random page access
    auto start = system_clock::now();
    for (size_t i = 0; i < PAGES_TO_WRITE; i++) {
        sum += index->data[random_lookups[i]];
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return  duration<double, std::milli>(end - start).count()/PAGES_TO_WRITE;
}

double CostModel::swap_cost() {
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        index->data[i] = rand() % 10000;
        index->data[i] = i;
    }
    node->current_start = 0;
    node->current_end = ELEMENT_COUNT - 1;

    //! Amount of remaining swaps
    ssize_t remaining_swaps = ELEMENT_COUNT;
    auto start = system_clock::now();

    //! swapping
    while (node->current_start < node->current_end && remaining_swaps > 0) {
        int64_t start = index->data[node->current_start];
        int64_t end = index->data[node->current_end];
        size_t start_pointer = index->data[node->current_start];
        size_t end_pointer = index->data[node->current_end];

        int start_has_to_swap = start >= node->key;
        int end_has_to_swap = end < node->key;
        int has_to_swap = start_has_to_swap * end_has_to_swap;

        index->data[node->current_start] = !has_to_swap * start + has_to_swap * end;
        index->data[node->current_end] = !has_to_swap * end + has_to_swap * start;
        index->data[node->current_start] = !has_to_swap * start_pointer + has_to_swap * end_pointer;
        index->data[node->current_end] = !has_to_swap * end_pointer + has_to_swap * start_pointer;

        node->current_start += !start_has_to_swap + has_to_swap;
        node->current_end -= !end_has_to_swap + has_to_swap;
        remaining_swaps--;
    }
    auto end = system_clock::now();

    return  duration<double, std::milli>(end - start).count()/PAGES_TO_WRITE;
}
CostModel::CostModel(){
    //! Allocate everyone
    base_column = make_unique<Column>(ELEMENT_COUNT);
    index = make_unique<Column>(ELEMENT_COUNT);
    node = make_unique<KDNode>(5000, 0, ELEMENT_COUNT - 1);
    //! Write base Column
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        base_column->data[i] = rand() % 50000;
        base_column->data[i] = i;
    }

}
CostModel::~CostModel(){
    //! Clean Memory
    base_column.reset();
    index.reset();
    node.reset();
}

