#pragma once

#include <memory>
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

    void swap_cost_create(double& first_column_swap, double& extra_column_swap);
private:

    //! Original Column
    std::unique_ptr<Column> base_column;
    //! Original second Column
    std::unique_ptr<Column> base_column_2;
    //! Our index
    std::unique_ptr<Column> index;
    //! Our index (Second Column)
    std::unique_ptr<Column> index_2;
    //! Initial Pivot
    std::unique_ptr<KDNode> node;

    //! Query
    int64_t low = 1000;
    int64_t high = 20000;
    int64_t sum = 0;
};