#pragma once

#include <bits/unique_ptr.h>
#include <kd_tree/kd_node.hpp>
#include "column.hpp"

class CostModel{
public:
    CostModel();
    ~CostModel();
    size_t PAGESIZE = 4096;
    size_t ELEMENTS_PER_PAGE = PAGESIZE/sizeof(float);
    size_t PAGES_TO_WRITE = 100000;
    size_t ELEMENT_COUNT = PAGES_TO_WRITE * ELEMENTS_PER_PAGE;
    double write_sequential_page_cost();

    double read_sequential_with_matches_page_cost();

    double read_sequential_without_matches_page_cost();

    double read_random_access();

    double swap_cost();
private:

    //! Original Column
    std::unique_ptr<Column> base_column;

    //! Our index
    std::unique_ptr<Column> index;

    //! Initial Pivot
    std::unique_ptr<KDNode> node;

    //! Query
    int64_t low = 1000;
    int64_t high = 20000;
    int64_t sum = 0;
};