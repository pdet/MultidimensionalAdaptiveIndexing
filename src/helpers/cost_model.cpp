
#include <algorithm>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <bitvector.hpp>
#include "cost_model.hpp"

using namespace std;
using namespace chrono;

double CostModel::write_sequential_page_cost() {
    //! initial write
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        int matching = base_column->data[i] >= low && base_column->data[i] <= high;
        sum += matching * base_column->data[i];

        int bigger_pivot = base_column->data[i] >= node->key;
        int smaller_pivot = 1 - bigger_pivot;

        index->data[node->current_start] = base_column->data[i];
        index->data[node->current_end] = base_column->data[i];

        node->current_start += smaller_pivot;
        node->current_end -= bigger_pivot;
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
}

double CostModel::read_sequential_without_matches_page_cost() {
    //! reading
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        sum += index->data[i];
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
}

double CostModel::read_sequential_with_matches_page_cost() {
    auto start = system_clock::now();
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        int matching = index->data[i] >= low && index->data[i] <= high;
        sum += index->data[i] * matching;
    }
    auto end = system_clock::now();
    //! Avoiding -O3 Optimization
    if (sum != 0)
        fprintf(stderr, " ");
    return duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
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
    return duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
}

void CostModel::swap_cost_create(double& first_column_swap, double& extra_column_swap) {
    node->current_start = 0;
    node->current_end = ELEMENT_COUNT - 1;
    //! Amount of remaining swaps
    ssize_t remaining_swaps = ELEMENT_COUNT;
    auto start = system_clock::now();
    BitVector goDown = BitVector(remaining_swaps);
    //! swapping
    size_t bit_idx = 0;
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        int bigger_pivot = base_column->data[i] >= node->key;
        int smaller_pivot = 1 - bigger_pivot;
        index->data[node->current_start] = base_column->data[i];
        index->data[node->current_end] = base_column->data[i];
        goDown.set(bit_idx++, smaller_pivot);
        node->current_start += smaller_pivot;
        node->current_end -= bigger_pivot;
    }
    auto end = system_clock::now();

    first_column_swap =  duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
    start = system_clock::now();
    size_t initial_low_cur = 0;
    size_t initial_high_cur = ELEMENT_COUNT - 1;
        //! First we copy the elements of the other columns, until where we stopped skipping
        bit_idx = 0;
        for (size_t i = 0; i < ELEMENT_COUNT; i++) {
            index_2->data[initial_low_cur] = base_column_2->data[i];
            index_2->data[initial_high_cur] = base_column_2->data[i];
            initial_low_cur += goDown.get(bit_idx);
            initial_high_cur -= !goDown.get(bit_idx++);
        }

        end = system_clock::now();

   extra_column_swap = duration<double, std::milli>(end - start).count() / PAGES_TO_WRITE;
}

CostModel::CostModel() {
    //! Allocate everyone
    base_column = make_unique<Column>(ELEMENT_COUNT);
    base_column_2 = make_unique<Column>(ELEMENT_COUNT);
    index = make_unique<Column>(ELEMENT_COUNT);
    index_2 = make_unique<Column>(ELEMENT_COUNT);
    node = make_unique<KDNode>(5000, 0, ELEMENT_COUNT - 1);
    //! Write base Column
    for (size_t i = 0; i < ELEMENT_COUNT; i++) {
        base_column->data[i] = rand() % 50000;
        base_column_2->data[i] = rand() % 50000;
    }

}

CostModel::~CostModel() {
    //! Clean Memory
    base_column.reset();
    index.reset();
    node.reset();
}

