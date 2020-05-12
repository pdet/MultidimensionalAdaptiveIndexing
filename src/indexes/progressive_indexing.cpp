#include <chrono>
#include <iostream>
#include <bitvector.hpp>
#include "progressive_index.hpp"
#include "candidate_list.hpp"
#include "full_scan.hpp"

using namespace std;
using namespace chrono;

bool isTest = false;

//unique_ptr<RefinementScan> ProgressiveIndex::find_offsets(QSAVLTree* tree,IdxColEntry* column, int64_t low, int64_t high, size_t col_size){
//    auto offsets = make_unique<RefinementScan>();
//    //! Find left side
//    auto leftNode = tree->FindNodeGTE(low);
//    //! Find Right side
//    auto rightNode = tree->FindNodeLT(high);
//    if (leftNode) {
//        if (leftNode->sorted) {
//            auto pieceStart = tree->pieceStart(leftNode);
//            auto pieceEnd = tree->pieceEnd(leftNode);
//            offsets->offsetLeft = binary_search_gte(column, low, pieceStart->offset, pieceEnd->offset);
//            offsets->checkLeft = false;
//        } else {
//            auto pieceEnd = tree->pieceEnd(leftNode);
//            offsets-> offsetLeft = leftNode->current_start;
//            offsets->offsetLeftMiddle = pieceEnd->offset;
//        }
//    } else {
//        offsets->offsetLeft = 0;
//        auto firstPiece = tree->FindMin(tree->root.get());
//        offsets->offsetLeftMiddle = firstPiece->current_end;
//    }
//    if (rightNode) {
//        if (rightNode->sorted) {
//            offsets->checkRight = false;
//            auto pieceStart = tree->pieceStart(rightNode);
//            auto pieceEnd = tree->pieceEnd(rightNode);
//            offsets->offsetRight = binary_search_lte(column, high, pieceStart->offset, pieceEnd->offset);
//        } else {
//            auto pieceStart = tree->pieceStart(rightNode);
//            offsets->offsetRightMiddle = pieceStart->offset;
//            offsets->offsetRight = rightNode->current_end;
//        }
//    } else {
//        offsets->offsetRight = col_size - 1;
//        auto lastPiece = tree->FindMax(tree->root.get());
//        offsets->offsetRightMiddle = lastPiece->current_start;
//    }
//    return move(offsets);
//
//}
//
//ResultStruct ProgressiveIndex::refinement_scan(int64_t low, int64_t high, size_t col_size) {
//    ResultStruct result;
//    auto offsets = find_offsets(tree,column,low,high,col_size);
//    //! Both pieces are sorted
//    if (!offsets->checkLeft && !offsets->checkRight) {
//        for (size_t i = offsets->offsetLeft; i < offsets->offsetRight; i++) {
//            result.push_back(column[i]);
//        }
//    } else if (!offsets->checkLeft) { //! Only Left is Sorted
//        for (size_t i = offsets->offsetLeft; i < offsets->offsetRightMiddle; i++) {
//            result.push_back(column[i]);
//        }
//        //! We check the values of the right node
//        for (size_t i = offsets->offsetRightMiddle; i <= offsets->offsetRight; i++) {
//            int match = low <= column[i].m_key && column[i] < high;
//            result.maybe_push_back(column[i], match);
//        }
//    } else if (!offsets->checkRight) { //! Only right is sorted
//        for (size_t i = offsets->offsetLeft; i < offsets->offsetLeftMiddle; i++) {
//            int match = low <= column[i].m_key && column[i] < high;
//            result.maybe_push_back(column[i], match);
//        }
//        for (size_t i = offsets->offsetLeftMiddle; i < offsets->offsetRight; i++) {
//            result.push_back(column[i]);
//        }
//    } else { //! No sorted nodes
//        //! check if there are no middle pieces
//        if (offsets->offsetLeft == offsets->offsetRightMiddle || offsets->offsetLeftMiddle == offsets->offsetRight) {
//            for (size_t i = offsets->offsetLeft; i <= offsets->offsetLeftMiddle; i++) {
//                int match = low <= column[i].m_key && column[i] < high;
//                result.maybe_push_back(column[i], match);
//            }
//        } else { //! We have middle pieces
//            //! We only have one middle piece that has not finished pivoting
//            if (offsets->offsetLeftMiddle >= offsets->offsetRightMiddle) {
//                //! We have to match everything
//                for (size_t i = offsets->offsetLeft; i <= offsets->offsetRight; i++) {
//                    int match = low <= column[i].m_key && column[i] < high;
//                    result.maybe_push_back(column[i], match);
//                }
//            } else {
//                for (size_t i = offsets->offsetLeft; i <= offsets->offsetLeftMiddle; i++) {
//                    int match = low <= column[i].m_key && column[i] < high;
//                    result.maybe_push_back(column[i], match);
//                }
//                //! No need to match middle pieces
//                for (size_t i = offsets->offsetLeftMiddle + 1; i < offsets->offsetRightMiddle; i++) {
//                    result.push_back(column[i]);
//                }
//                for (size_t i = offsets->offsetRightMiddle; i <= offsets->offsetRight; i++) {
//                    int match = low <= column[i].m_key && column[i] < high;
//                    result.maybe_push_back(column[i], match);
//                }
//            }
//        }
//    }
//
//    return result;
//}

void ProgressiveIndex::progressive_quicksort_refine(Query &query, ssize_t &remaining_swaps) {
    size_t num_dimensions = query.predicate_count();
    while (!refinement_nodes->empty() && remaining_swaps) {
        auto node = refinement_nodes->at(node_being_refined);
        auto column = table->columns[node->column]->data;
        //! Now we swap everything related to this node
        while (node->current_start < node->current_end && remaining_swaps > 0) {
            auto start = column[node->current_start];
            auto end = column[node->current_end];
            int start_has_to_swap = start >= node->key;
            int end_has_to_swap = end < node->key;
            int has_to_swap = start_has_to_swap * end_has_to_swap;
            if (has_to_swap) {
                remaining_swaps--;
                table->exchange(node->current_start, node->current_end);
            }
            node->current_start += !start_has_to_swap + has_to_swap;
            node->current_end -= !end_has_to_swap + has_to_swap;
        }
        //! Did we finish pivoting this node?
        if (node->current_start < node->current_end && !node->finished) {
            node_being_refined++;
            size_t next_dimension = node->column == num_dimensions - 1 ? 0 : node->column + 1;
            column = table->columns[next_dimension]->data;
            //! We need to create children
            //! construct the left and right side of the root node on next dimension
            int64_t pivot = column[node->current_start / 2];
            size_t current_start = 0;
            size_t current_end = node->current_end;
            node->setLeft(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
            if (current_end - current_start <= minimum_partition_size) {
                node->left_child->finished = true;
            }

            //! Right node
            pivot = column[(node->current_start + node->end) / 2];
            current_start = current_end + 1;
            current_end = node->end - 1;
            node->setRight(make_unique<KDNode>(next_dimension, pivot, current_start, current_end));
            if (current_end - current_start <= minimum_partition_size) {
                node->right_child->finished = true;
            }
            refinement_nodes->push_back(node->left_child.get());
            refinement_nodes->push_back(node->right_child.get());
            //! is the  children size lower than threshold?
        }
    }
}

unique_ptr<Table>
ProgressiveIndex::progressive_quicksort_create(Table *originalTable, Query &query, ssize_t &remaining_swaps) {
    auto root = tree->root.get();
    //! Creation Phase only partitions first dimension
    size_t dim = 0;
    size_t table_size = originalTable->row_count();
    auto low = query.predicates[dim].low;
    auto high = query.predicates[dim].high;
    auto indexColumn = table->columns[dim]->data;
    auto originalColumn = originalTable->columns[dim]->data;
    //! If we go up or down for next filters
    BitVector goDown = BitVector(remaining_swaps);
    //! Candidate Lists from Index
    CandidateList up;
    //! for the initial run, we write the indices instead of swapping them
    //! because the current array has not been initialized yet
    //! first look through the part we have already pivoted
    //! for data that matches the points
    //! We start by getting a candidate list to the upper part of our indexed table
    if (low <= root->key) {
        for (size_t i = 0; i < root->current_start; i++) {
            int matching = indexColumn[i] >= low && indexColumn[i] <= high;
            up.maybe_push_back(i, matching);
        }
        for (dim = 1; dim < query.predicate_count(); ++dim) {
            if (up.size == 0) {
                break;
            }
            CandidateList new_up(up.size);
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            indexColumn = table->columns[dim]->data;
            for (size_t i = 0; i < up.size; i++) {
                int matching = indexColumn[up.get(i)] >= low && indexColumn[up.get(i)] <= high;
                new_up.maybe_push_back(up.get(i), matching);
            }
            up.initialize(new_up);
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
            CandidateList new_down(down.size);
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            indexColumn = table->columns[dim]->data;
            for (size_t i = 0; i < down.size; i++) {
                int matching = indexColumn[down.get(i)] >= low && indexColumn[down.get(i)] <= high;
                new_down.maybe_push_back(down.get(i), matching);
            }
            down.initialize(new_down);
        }
    }
    //! Now we start filling our candidate list that points to the original table
    //! It has elements from when we start swapping in this partition till the end of the table
    //! Here we use a bitvector instead of a candidate list
    BitVector mid_bit_vec = BitVector(remaining_swaps);
    dim = 0;
    low = query.predicates[dim].low;
    high = query.predicates[dim].high;
    indexColumn = table->columns[dim]->data;
    //! now we start filling the index with at most remaining_swap entries
    size_t initial_low = root->current_start;
    size_t initial_current_pos = current_position;
    size_t next_index = min(current_position + remaining_swaps, table_size);
    size_t initial_high = root->current_end;
    remaining_swaps -= next_index - current_position;
    size_t bit_idx = 0;
    for (size_t i = current_position; i < next_index; i++) {
        int matching = originalColumn[i] >= low && originalColumn[i] <= high;
        mid_bit_vec.set(bit_idx, matching);
        int bigger_pivot = indexColumn[i] >= root->key;
        int smaller_pivot = 1 - bigger_pivot;

        indexColumn[root->current_start] = originalColumn[i];
        indexColumn[root->current_end] = originalColumn[i];
        goDown.set(bit_idx++, smaller_pivot);
        root->current_start += smaller_pivot;
        root->current_end -= bigger_pivot;
    }
    for (dim = 1; dim < query.predicate_count(); ++dim) {
        low = query.predicates[dim].low;
        high = query.predicates[dim].high;
        indexColumn = table->columns[dim]->data;
        originalColumn = originalTable->columns[dim]->data;
        size_t initial_low_cur = initial_low;
        size_t initial_high_cur = initial_high;
        //! First we copy the elements of the other columns, until where we stopped skipping
        bit_idx = 0;
        for (size_t i = current_position; i < next_index; i++) {
            if (mid_bit_vec.get(bit_idx)) {
                int matching = originalColumn[i] >= low && originalColumn[i] <= high;
                mid_bit_vec.set(bit_idx, matching);
            }
            indexColumn[initial_low_cur] = originalColumn[i];
            indexColumn[initial_high_cur] = originalColumn[i];
            initial_low_cur += goDown.get(bit_idx);
            initial_high_cur -= goDown.get(bit_idx++);
        }
    }
    current_position = next_index;
    //! Check if we are finished with the initial run
    CandidateList original;
    if (next_index == table_size) {
        dim = 1;
        indexColumn = table->columns[dim]->data;
        assert(root->current_start >= root->current_end);
        //! construct the left and right side of the root node on next dimension
        int64_t pivot = indexColumn[root->current_start / 2];
        size_t current_start = 0;
        size_t current_end = root->current_end;

        root->setLeft(make_unique<KDNode>(dim,pivot, current_start, current_end));

        //! Right node
        pivot = indexColumn[(root->current_start + table_size) / 2];
        current_start = current_end + 1;
        current_end = table_size - 1;
        root->setRight(make_unique<KDNode>(dim,pivot, current_start, current_end));
        refinement_nodes->push_back(root->left_child.get());
        refinement_nodes->push_back(root->right_child.get());
    } else {
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
            CandidateList new_original(original.size);
            low = query.predicates[dim].low;
            high = query.predicates[dim].high;
            originalColumn = originalTable->columns[dim]->data;
            for (size_t i = 0; i < original.size; i++) {
                int matching = originalColumn[original.get(i)] >= low && originalColumn[original.get(i)] <= high;
                new_original.maybe_push_back(original.get(i), matching);
            }
            original.initialize(new_original);
        }
    }
    //! Now we create the results
    //! Iterate candidate lists that point to index
    double sum = 0;
    float count = up.size + down.size + original.size;
    dim = 0;
    originalColumn = originalTable->columns[dim]->data;
    indexColumn = table->columns[dim]->data;
    for (size_t i = 0; i < up.size; i++) {
        sum += indexColumn[up.get(i)];
    }
    for (size_t i = 0; i < down.size; i++) {
        sum += indexColumn[down.get(i)];
    }
    for (size_t i = 0; i < mid_bit_vec.size(); i++) {
        if (mid_bit_vec.get(i)) {
            count++;
            sum += originalColumn[initial_current_pos + i];
        }
    }
    for (size_t i = 0; i < original.size; i++) {
        sum += originalColumn[original.get(i)];
    }
    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(sum), static_cast<float>(count)};
    t->append(row);
    return t;
}

unique_ptr<Table> ProgressiveIndex::progressive_quicksort(Table *originalTable, Query &query) {
    //! Amount of work we are allowed to do
    ssize_t remaining_swaps = (ssize_t) (originalTable->row_count() * delta);

    //! Creation Phase
    //! If the node has no children we are stil in the creation phase
    assert(tree->root);
    if (tree->root->noChildren()) {
        //! Creation Phase
        return progressive_quicksort_create(originalTable, query, remaining_swaps);
    } else if (!tree->root->finished) { //! If the root is not marked as sort we still have refinement to do!
        //! Refinement Phase
        progressive_quicksort_refine(query, remaining_swaps);
        auto search_results= tree->search(query);
        auto partitions = search_results.first;
        auto partition_skip = search_results.second;
        auto result = FullScan::scan_partition(table.get(), query,partitions, partition_skip);
        auto t = make_unique<Table>(2);
        float row[2] = {static_cast<float>(result.first), static_cast<float>(result.second)};
        t->append(row);
        return t;

    }
    //! We are in the consolidation phase no more indexing to be done, just scan it.
}

ProgressiveIndex::ProgressiveIndex(std::map<std::string, std::string> config) {
    if (config.find("minimum_partition_size") == config.end())
        minimum_partition_size = 100;
    else
        minimum_partition_size = std::stoi(config["minimum_partition_size"]);
}

ProgressiveIndex::~ProgressiveIndex() {}

double ProgressiveIndex::get_costmodel_delta_quicksort(vector<int64_t> &originalColumn, int64_t low, int64_t high,
                                                       double delta) {
    return 0.0;
}

//! Here we just malloc the table and initialize the root
void ProgressiveIndex::initialize(Table *table_to_copy) {
    auto start = measurements->time();
    table = make_unique<Table>(table_to_copy->col_count(), table_to_copy->row_count());
    initializeRoot(table_to_copy->row_count() / 2, table_to_copy->row_count());
    auto end = measurements->time();
    measurements->append(
            "initialization_time",
            std::to_string(Measurements::difference(end, start))
    );
}

//! Here we don't do anything since adapt and scan are the same process in progressive indexing
void ProgressiveIndex::adapt_index(Table *originalTable, Query &query) {
    return;
}

unique_ptr<Table> ProgressiveIndex::range_query(Table *originalTable, Query &query) {
    return progressive_quicksort(originalTable, query);
}
