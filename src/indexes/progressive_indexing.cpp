#include <chrono>
#include <iostream>
#include <bitvector.hpp>
#include <cost_model.hpp>
#include <full_scan_candidate_list.hpp>
#include "progressive_index.hpp"
#include "candidate_list.hpp"
#include "full_scan.hpp"

using namespace std;
using namespace chrono;

double scan_time = 0;
double adaptation_time = 0;
double index_search_time = 0;
double start_time = 0;
double end_time = 0;

//! Bit hacky, this can be done while reading the prev piece
float find_avg(Table *table, size_t col_idx, size_t start, size_t end) {
    double sum = 0;
    size_t total = end - start;
    for (; start < end; start++) {
        sum += table->columns[col_idx]->data[start];
    }
    return sum / total;
}

//! Workload agnostic is only used if we already refined dependent on workload or we already answered
//! the query
void ProgressiveIndex::workload_agnostic_refine(Query &query, ssize_t &remaining_swaps) {
    size_t num_dimensions = query.predicate_count();
    while (node_being_refined < refinement_nodes->size() && remaining_swaps) {
        auto node = refinement_nodes->at(node_being_refined);
        while (node->current_start >= node->current_end) {
            node_being_refined++;
            node = refinement_nodes->at(node_being_refined);
        }

        auto column = table->columns[node->column]->data;
        //! Now we swap everything related to this node
        start_time = measurements->time();
        while (node->current_start < node->current_end && remaining_swaps > 0) {
            auto start = column[node->current_start];
            auto end = column[node->current_end];
            int start_has_to_swap = start >= node->key;
            int end_has_to_swap = end < node->key;
            int has_to_swap = start_has_to_swap * end_has_to_swap;
            if (has_to_swap) {
                table->exchange(node->current_start, node->current_end);
            }
            remaining_swaps--;
            node->current_start += !start_has_to_swap + has_to_swap;
            node->current_end -= !end_has_to_swap + has_to_swap;
        }
        end_time = measurements->time();
        adaptation_time += end_time - start_time;
        //! Did we finish pivoting this node?
        if (node->current_start >= node->current_end) {
            node->position = column[node->current_start] >= node->key ? node->current_start : node->current_start + 1;
            node_being_refined++;
            size_t next_dimension = node->column == num_dimensions - 1 ? 0 : node->column + 1;
            //! We need to create children
            //! construct the left and right side of the root node on next dimension
            size_t current_start = node->start;
            size_t current_end = node->position == 0 ? 0 : node->position - 1;
            if (current_end >= current_start && current_end - current_start >= minimum_partition_size) {
                float pivot = find_avg(table.get(), next_dimension, current_start, current_end);
                node->setLeft(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
                refinement_nodes->push_back(node->left_child.get());
            }
            //! Right node
            current_start = node->position;
            current_end = node->end;
            if (current_end >= current_start && current_end - current_start >= minimum_partition_size) {
                float pivot = find_avg(table.get(), next_dimension, current_start, current_end);
                node->setRight(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
                refinement_nodes->push_back(node->right_child.get());
            }
        }
    }
//    cout << node_being_refined << " of : " << refinement_nodes->size() << endl;
    if (node_being_refined >= refinement_nodes->size()-1) {
//        cout << " COnverged";
        converged = true;
    }
}

void ProgressiveIndex::workload_dependent_refine(Query &query, ssize_t &remaining_swaps) {
    start_time = measurements->time();
    vector<KDNode *> vip_nodes;
    tree->search_nodes(query, vip_nodes);
    end_time = measurements->time();
    index_search_time += end_time - start_time;
    size_t vip_node_idx = 0;
    size_t num_dimensions = query.predicate_count();
    while (vip_node_idx < vip_nodes.size() && remaining_swaps) {
        auto node = vip_nodes.at(vip_node_idx);
        auto column = table->columns[node->column]->data;
        //! Now we swap everything related to this node
        start_time = measurements->time();
        while (node->current_start < node->current_end && remaining_swaps > 0) {
            auto start = column[node->current_start];
            auto end = column[node->current_end];
            int start_has_to_swap = start >= node->key;
            int end_has_to_swap = end < node->key;
            int has_to_swap = start_has_to_swap * end_has_to_swap;
            if (has_to_swap) {
                table->exchange(node->current_start, node->current_end);
            }
            remaining_swaps--;
            node->current_start += !start_has_to_swap + has_to_swap;
            node->current_end -= !end_has_to_swap + has_to_swap;
        }
        end_time = measurements->time();
        adaptation_time += end_time - start_time;
        //! Did we finish pivoting this node?
        if (node->current_start >= node->current_end) {
            node->position = column[node->current_start] >= node->key ? node->current_start : node->current_start + 1;
            vip_node_idx++;
            size_t next_dimension = node->column == num_dimensions - 1 ? 0 : node->column + 1;
            //! We need to create children
            //! construct the left and right side of the root node on next dimension
            size_t current_start = node->start;
            size_t current_end = node->position == 0 ? 0 : node->position - 1;
            if (current_end >= current_start && current_end - current_start >= minimum_partition_size) {
                float pivot = find_avg(table.get(), next_dimension, current_start, current_end);
                node->setLeft(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
                vip_nodes.push_back(node->left_child.get());
                refinement_nodes->push_back(node->left_child.get());
            }
            //! Right node
            current_start = node->position;
            current_end = node->end;
            if (current_end >= current_start && current_end - current_start >= minimum_partition_size) {
                float pivot = find_avg(table.get(), next_dimension, current_start, current_end);
                node->setRight(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
                vip_nodes.push_back(node->right_child.get());
                refinement_nodes->push_back(node->right_child.get());
            }
        }
    }
    //! This means we fully refined all nodes related to the query, now we can use the remaining budget to refine
    //! random nodes
    if (remaining_swaps > 0) {
        workload_agnostic_refine(query, remaining_swaps);
    }
}

unique_ptr<Table>
ProgressiveIndex::progressive_quicksort_create(Query &query, ssize_t &remaining_swaps) {
    //! for the initial run, we write the indices instead of swapping them
    //! because the current array has not been initialized yet
    //! first look through the part we have already pivoted
    //! for data that matches the points
    //! We start by getting a candidate list to the upper part of our indexed table
    start_time = measurements->time();
    auto root = tree->root.get();
    //! Creation Phase only partitions first dimension
    size_t dim = 0;
    size_t table_size = originalTable->row_count();
    auto low = query.predicates[dim].low;
    auto high = query.predicates[dim].high;
    auto indexColumn = table->columns[dim]->data;
    auto originalColumn = originalTable->columns[dim]->data;

    //! for the initial run, we write the indices instead of swapping them
    //! because the current array has not been initialized yet
    //! first look through the part we have already pivoted
    //! for data that matches the points
    //! We start by getting a candidate list to the upper part of our indexed table
    start_time = measurements->time();
    //! Candidate Lists from Index
    CandidateList up;
    if (low <= root->key) {
        for (size_t i = 0; i < root->current_start; i++) {
            int matching = indexColumn[i] >= low && indexColumn[i] <= high;
            up.maybe_push_back(i, matching);
        }
        for (dim = 1; dim < query.predicate_count(); ++dim) {
            if (up.size == 0) {
                break;
            }
            auto qualifying_index = 0;
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            indexColumn = table->columns[dim]->data;
            for (size_t i = 0; i < up.size; i++) {
                int matching = indexColumn[up.get(i)] >= low && indexColumn[up.get(i)] <= high;
                up.data[qualifying_index] = up.get(i);
                qualifying_index += matching;
            }
            up.size = qualifying_index;
        }
    }

    CandidateList down;
    //! We now get a candidate list to the bottom part of our indexed table
    if (high >= root->key) {
        dim = 0;
        low = query.predicates[dim].low;
        high = query.predicates[dim].high;
        indexColumn = table->columns[dim]->data;
        for (size_t i = root->current_end + 1; i < table_size; i++) {
            int matching = indexColumn[i] >= low && indexColumn[i] <= high;
            down.maybe_push_back(i, matching);
        }
        for (dim = 1; dim < query.predicate_count(); ++dim) {
            if (down.size == 0) {
                break;
            }
            auto qualifying_index = 0;
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            indexColumn = table->columns[dim]->data;
            for (size_t i = 0; i < down.size; i++) {
                int matching = indexColumn[down.get(i)] >= low && indexColumn[down.get(i)] <= high;
                down.data[qualifying_index] = down.get(i);
                qualifying_index += matching;
            }
            down.size = qualifying_index;
        }
    }
    end_time = measurements->time();
    scan_time += end_time - start_time;
    //! Here we calculate how much indexing we can do here
    if (interactivity_threshold > 0 && (tree->root->start != tree->root->current_start
                                        || tree->root->end != tree->root->current_end)) {
        remaining_swaps = table->row_count() * get_delta(query);
    }

    //! Now we start filling our candidate list that points to the original table
    //! It has elements from when we start swapping in this partition till the end of the table
    start_time = measurements->time();
    //! If we go up or down for next filters
    dim = 0;
    low = query.predicates[dim].low;
    high = query.predicates[dim].high;
    indexColumn = table->columns[dim]->data;
    //! now we start filling the index with at most remaining_swap entries
    size_t initial_low = root->current_start;
    size_t next_index = min(current_position + remaining_swaps, table_size);
    size_t initial_high = root->current_end;
    remaining_swaps -= next_index - current_position;
    size_t bit_idx = 0;
    CandidateList mid;
    BitVector goDown = BitVector(next_index - current_position);
    for (size_t i = current_position; i < next_index; i++) {
        int matching = originalColumn[i] >= low && originalColumn[i] <= high;
        mid.maybe_push_back(i, matching);
        int bigger_pivot = originalColumn[i] >= root->key;
        int smaller_pivot = 1 - bigger_pivot;

        indexColumn[root->current_start] = originalColumn[i];
        indexColumn[root->current_end] = originalColumn[i];
        goDown.set(bit_idx++, smaller_pivot);
        root->current_start += smaller_pivot;
        root->current_end -= bigger_pivot;
    }
    end_time = measurements->time();
    adaptation_time += end_time - start_time;
    start_time = measurements->time();
    for (dim = 1; dim < query.predicate_count(); ++dim) {
        low = query.predicates[dim].low;
        high = query.predicates[dim].high;
        indexColumn = table->columns[dim]->data;
        originalColumn = originalTable->columns[dim]->data;
        size_t initial_low_cur = initial_low;
        size_t initial_high_cur = initial_high;
        //! First we copy the elements of the other columns, until where we stopped skipping
        bit_idx = 0;
        auto mid_idx = 0;
        auto qualifying_idx = 0;
        for (size_t i = current_position; i < next_index; i++) {
            int matching = originalColumn[i] >= low && originalColumn[i] <= high;
            int cur_pos_match = i == mid.get(mid_idx);
            mid.data[qualifying_idx] = mid.get(mid_idx);
            qualifying_idx += matching * cur_pos_match;
            mid_idx += cur_pos_match;

            indexColumn[initial_low_cur] = originalColumn[i];
            indexColumn[initial_high_cur] = originalColumn[i];
            initial_low_cur += goDown.get(bit_idx);
            initial_high_cur -= !goDown.get(bit_idx++);
        }
        mid.size = qualifying_idx;
    }
    //! Check if we are finished with the initial run
    CandidateList original;
    end_time = measurements->time();
    adaptation_time += end_time - start_time;
    current_position = next_index;

    if (next_index == table_size) {
        indexColumn = table->columns[0]->data;
        root->position = indexColumn[root->current_start] >= root->key ? root->current_start : root->current_start + 1;
        assert(indexColumn[root->position - 1] < root->key);
        assert(indexColumn[root->position] >= root->key);
        assert(indexColumn[root->position + 1] >= root->key);
        dim = 1;
        //! construct the left and right side of the root node on next dimension
        float pivot = find_avg(table.get(), dim, 0, root->current_start);
        size_t current_start = 0;
        size_t current_end = root->current_start - 1;
        root->position = root->current_start;
        root->setLeft(make_unique<KDNode>(dim, pivot, current_start, current_end));

        //! Right node
        pivot = find_avg(table.get(), dim, current_start + 1, table_size - 1);
        current_start = root->current_start;
        current_end = table_size - 1;
        root->setRight(make_unique<KDNode>(dim, pivot, current_start, current_end));
        refinement_nodes->push_back(root->left_child.get());
        refinement_nodes->push_back(root->right_child.get());
    } else {
        start_time = measurements->time();
        //! we have done all the swapping for this run
        //! now we query the remainder of the data
        dim = 0;
        low = query.predicates[dim].low;
        high = query.predicates[dim].high;
        originalColumn = originalTable->columns[dim]->data;
        for (size_t i = current_position; i < table_size; i++) {
            int matching = originalColumn[i] >= low && originalColumn[i] <= high;
            original.maybe_push_back(i, matching);
        }
        for (dim = 1; dim < query.predicate_count(); ++dim) {
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            originalColumn = originalTable->columns[dim]->data;
            auto qualifying_index = 0;
            for (size_t i = 0; i < original.size; i++) {
                int matching = originalColumn[original.get(i)] >= low && originalColumn[original.get(i)] <= high;
                original.data[qualifying_index] = original.get(i);
                qualifying_index += matching;
            }
            original.size = qualifying_index;

        }
        end_time = measurements->time();
        scan_time += end_time - start_time;
    }
    //! Now we create the results
    //! Iterate candidate lists that point to index
    start_time = measurements->time();
    double sum = 0;
    size_t count = up.size + down.size + original.size + mid.size;
    dim = 0;
    originalColumn = originalTable->columns[dim]->data;
    indexColumn = table->columns[dim]->data;
    for (size_t i = 0; i < up.size; i++) {
        sum += indexColumn[up.get(i)];
    }
    for (size_t i = 0; i < down.size; i++) {
        sum += indexColumn[down.get(i)];
    }
    for (size_t i = 0; i < mid.size; i++) {
        sum += originalColumn[mid.get(i)];
    }
    for (size_t i = 0; i < original.size; i++) {
        sum += originalColumn[original.get(i)];
    }
    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(sum), static_cast<float>(count)};
    t->append(row);
    end_time = measurements->time();
    scan_time += end_time - start_time;
    //! Here we react in case we can still perform indexing
    if (interactivity_threshold > 0 && (root->start != initial_low
                                        || root->end != initial_high) && root->noChildren()) {
        while (scan_time + adaptation_time + index_search_time < 0.9 * interactivity_threshold && root->noChildren()) {
            assert (root->current_end >= root->current_start-1);
            remaining_swaps = table->row_count() * get_delta_react();
            start_time = measurements->time();
            initial_low = root->current_start;
            next_index = min(current_position + remaining_swaps, table_size);
            initial_high = root->current_end;
            dim = 0;
            originalColumn = originalTable->columns[dim]->data;
            indexColumn = table->columns[dim]->data;
            bit_idx = 0;
            goDown = BitVector(next_index - current_position);
            for (size_t i = current_position; i < next_index; i++) {
                int bigger_pivot = originalColumn[i] >= root->key;
                int smaller_pivot = 1 - bigger_pivot;
                indexColumn[root->current_start] = originalColumn[i];
                indexColumn[root->current_end] = originalColumn[i];
                goDown.set(bit_idx++, smaller_pivot);
                root->current_start += smaller_pivot;
                root->current_end -= bigger_pivot;
            }
            for (dim = 1; dim < query.predicate_count(); ++dim) {
                indexColumn = table->columns[dim]->data;
                originalColumn = originalTable->columns[dim]->data;
                size_t initial_low_cur = initial_low;
                size_t initial_high_cur = initial_high;
                //! First we copy the elements of the other columns, until where we stopped skipping
                bit_idx = 0;
                for (size_t i = current_position; i < next_index; i++) {
                    indexColumn[initial_low_cur] = originalColumn[i];
                    indexColumn[initial_high_cur] = originalColumn[i];
                    initial_low_cur += goDown.get(bit_idx);
                    initial_high_cur -= !goDown.get(bit_idx++);
                }
            }
            end_time = measurements->time();
            adaptation_time += end_time - start_time;
            current_position = next_index;
            if (next_index == table_size) {
                indexColumn = table->columns[0]->data;
                root->position =
                        indexColumn[root->current_start] >= root->key ? root->current_start : root->current_start + 1;
                assert(indexColumn[root->position - 1] < root->key);
                assert(indexColumn[root->position] >= root->key);
                assert(indexColumn[root->position + 1] >= root->key);
                dim = 1;
                //! construct the left and right side of the root node on next dimension
                float pivot = find_avg(table.get(), dim, 0, root->current_start);
                size_t current_start = 0;
                size_t current_end = root->current_start - 1;
                root->position = root->current_start;
                root->setLeft(make_unique<KDNode>(dim, pivot, current_start, current_end));
                //! Right node
                pivot = find_avg(table.get(), dim, current_start + 1, table_size - 1);
                current_start = root->current_start;
                current_end = table_size - 1;
                root->setRight(make_unique<KDNode>(dim, pivot, current_start, current_end));
                refinement_nodes->push_back(root->left_child.get());
                refinement_nodes->push_back(root->right_child.get());
            }
        }
    }
    return t;
}

unique_ptr<Table> ProgressiveIndex::progressive_quicksort(Query &query) {
    //! Amount of work we are allowed to do
    auto remaining_swaps = (ssize_t) (originalTable->row_count() * delta);
    scan_time = 0;
    adaptation_time = 0;
    index_search_time = 0;
    //! Creation Phase
    //! If the node has no children we are stil in the creation phase
    assert(tree->root);
    //! If we are using the cost model our interactivity threshold will be the cost of the first query with delta.
    if (tree->root->start == tree->root->current_start && tree->root->end == tree->root->current_end &&
        interactivity_threshold > 0) {
        auto result = progressive_quicksort_create(query, remaining_swaps);
        interactivity_threshold = scan_time + index_search_time + adaptation_time;
        measurements->append(
                "scan_time",
                std::to_string(scan_time)
        );
        measurements->append(
                "index_search_time",
                std::to_string(index_search_time)
        );
        measurements->append(
                "adaptation_time",
                std::to_string(adaptation_time)
        );
        return result;
    }
    if (tree->root->noChildren()) {
        //! Creation Phase
        auto result = progressive_quicksort_create(query, remaining_swaps);
        //! In the last creation phase iteration we might have some swaps left
        if (remaining_swaps > 0) {
            workload_agnostic_refine(query, remaining_swaps);
            if (interactivity_threshold > 0) {
                while (scan_time + adaptation_time + index_search_time < 0.9 * interactivity_threshold && !converged) {
                    remaining_swaps = table->row_count() * get_delta_react();
                    //! Gotta do some refinements
                    workload_agnostic_refine(query, remaining_swaps);
                }
            }
        }
        measurements->append(
                "scan_time",
                std::to_string(scan_time)
        );
        measurements->append(
                "index_search_time",
                std::to_string(index_search_time)
        );
        measurements->append(
                "adaptation_time",
                std::to_string(adaptation_time)
        );
        return result;
    } else if (!converged) {
        if (interactivity_threshold > 0) {
            remaining_swaps = table->row_count() * get_delta(query);
        }
        //! Gotta do some refinements.
        workload_dependent_refine(query, remaining_swaps);
    }
    //! Index Lookup + Partition Scan
    start_time = measurements->time();
    auto search_results = tree->search(query);
    end_time = measurements->time();
    index_search_time += end_time - start_time;
    auto partitions = search_results.first;
    auto partition_skip = search_results.second;
    start_time = measurements->time();
    auto result = FullScanCandidateList::scan_partition(table.get(), query, partitions, partition_skip);
    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
    t->append(row);
    end_time = measurements->time();
    scan_time += end_time - start_time;
    if (!converged && interactivity_threshold > 0) {
        while (scan_time + adaptation_time + index_search_time < 0.9 * interactivity_threshold && !converged) {
            remaining_swaps = table->row_count() * get_delta_react();
            //! Gotta do some refinements, we have not converged yet.
            workload_agnostic_refine(query, remaining_swaps);
        }
    }
    measurements->append(
            "scan_time",
            std::to_string(scan_time)
    );
    measurements->append(
            "index_search_time",
            std::to_string(index_search_time)
    );
    measurements->append(
            "adaptation_time",
            std::to_string(adaptation_time)
    );
    return t;
}


ProgressiveIndex::ProgressiveIndex(std::map<std::string, std::string> config) {
    refinement_nodes = make_unique<vector<KDNode *>>();
    if (config.find("minimum_partition_size") == config.end()) {
        minimum_partition_size = 1024;
    } else {
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
    }

    if (config.find("delta") == config.end()) {
        delta = 0.2;
    } else {
        delta = std::stod(config["delta"]);
    }

    if (config.find("workload_adaptive") == config.end()) {
        workload_adaptive = false;
    } else {
        workload_adaptive = std::stoi(config["workload_adaptive"]) == 1;
    }
    if (config.find("interactivity_threshold") == config.end()) {
        interactivity_threshold = 0;
    } else {
        interactivity_threshold = std::stod(config["interactivity_threshold"]);
    }
    if (interactivity_threshold > 0) {
        //! We must get the costmodel constants
        CostModel cost_model;
        WRITE_ONE_PAGE_SEQ_MS = cost_model.write_sequential_page_cost();
        READ_ONE_PAGE_SEQ_MS = cost_model.read_sequential_with_matches_page_cost();
        READ_ONE_PAGE_WITHOUT_CHECKS_SEQ_MS = cost_model.read_sequential_without_matches_page_cost();
        RANDOM_ACCESS_PAGE_MS = cost_model.read_random_access();
        cost_model.swap_cost_create(first_col_cr, extra_col_cr);
    }

}

ProgressiveIndex::~ProgressiveIndex() = default;

double ProgressiveIndex::get_delta(Query &query) {
    double page_count =
            (table->row_count() / ELEMENTS_PER_PAGE) + (table->row_count() % ((int) ELEMENTS_PER_PAGE) != 0 ? 1 : 0);
    size_t ITERATIONS = 20;
    double estimated_delta = 0.5;
    double offset = estimated_delta / 2;
    double estimated_time;
    //! How much time we still have
    double time = interactivity_threshold - (scan_time + adaptation_time + index_search_time);
    double scan_speed =
            READ_ONE_PAGE_SEQ_MS * page_count * table->col_count() + RANDOM_ACCESS_PAGE_MS * table->col_count();
    double pivot_speed = (READ_ONE_PAGE_SEQ_MS + WRITE_ONE_PAGE_SEQ_MS) * page_count * table->col_count() +
                         RANDOM_ACCESS_PAGE_MS * table->col_count();
    //! Creation Phase
    if (tree->root->noChildren()) {
        //! figure out unindexed fraction
        double rho = (double) (table->row_count() - tree->root->current_start -
                               (tree->root->end - tree->root->current_end)) / (double) table->row_count();
        for (size_t j = 0; j < ITERATIONS; j++) {
            estimated_time = ((rho - estimated_delta) * scan_speed + estimated_delta * pivot_speed) / 1000.0;
            if (estimated_time > time) {
                estimated_delta -= offset;
            } else {
                estimated_delta += offset;
            }
            offset /= 2;
        }
        return estimated_delta;
    } else {
        size_t height = tree->get_max_height();
        double lookup_speed = height * RANDOM_ACCESS_PAGE_MS;
        double refine_speed = WRITE_ONE_PAGE_SEQ_MS * page_count * table->col_count() +
                              RANDOM_ACCESS_PAGE_MS * table->col_count();
        //! figure out alpha
        auto offsets = tree->search(query);
        double size = 0;
        for (auto &partition: offsets.first) {
            size += partition.second - partition.first;
        }
        double alpha = (size) / (double) table->row_count();

        for (size_t j = 0; j < ITERATIONS; j++) {
            estimated_time = (lookup_speed + alpha * scan_speed + estimated_delta * refine_speed) / 1000.0;
            if (estimated_time > time) {
                estimated_delta -= offset;
            } else {
                estimated_delta += offset;
            }
            offset /= 2;
        }
        return estimated_delta;
    }
}

double ProgressiveIndex::get_delta_react() {
    double page_count =
            (table->row_count() / ELEMENTS_PER_PAGE) + (table->row_count() % ((int) ELEMENTS_PER_PAGE) != 0 ? 1 : 0);
    //! Creation Phase
    if (tree->root->noChildren()) {
        double pivot_speed = first_col_cr * page_count +
                             extra_col_cr * table->col_count() - 1;
        double time = interactivity_threshold - (scan_time + adaptation_time + index_search_time);
        if (time < 0) {
            return 0;
        }
        return time * 100 / pivot_speed;
    }

    double pivot_speed = (READ_ONE_PAGE_SEQ_MS + WRITE_ONE_PAGE_SEQ_MS) * page_count * table->col_count() +
                         RANDOM_ACCESS_PAGE_MS * table->col_count();
    double time = interactivity_threshold - (scan_time + adaptation_time + index_search_time);
    if (time < 0) {
        return 0;
    }
    return time * 1000 / pivot_speed;
}

//! Here we just malloc the table and initialize the root
void ProgressiveIndex::initialize(Table *table_to_copy) {
    //! Check partition size, we change it to guarantee it always partitions all dimensions at least once
    while (minimum_partition_size > table_to_copy->row_count() / pow(2, table_to_copy->col_count())) {
        minimum_partition_size /= 2;
    }
    if (minimum_partition_size < 100) {
        minimum_partition_size = 100;
    }
    //! We don't time this because could be assume this is collected during data loading
    float pivot = find_avg(table_to_copy, 0, 0, table_to_copy->row_count());
    auto start = measurements->time();
    table = make_unique<Table>(table_to_copy->col_count(), table_to_copy->row_count());
    originalTable = table_to_copy;
    initializeRoot(pivot, table_to_copy->row_count());
    auto end = measurements->time();
    measurements->append(
            "initialization_time",
            std::to_string(Measurements::difference(end, start))
    );
}

//! Here we don't do anything since adapt and scan are the same process in progressive indexing
void ProgressiveIndex::adapt_index(Query &query) {}

unique_ptr<Table> ProgressiveIndex::range_query(Query &query) {
    return progressive_quicksort(query);
}
