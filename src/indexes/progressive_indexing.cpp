#include <chrono>
#include <iostream>
#include <bitvector.hpp>
#include "progressive_index.hpp"
#include "candidate_list.hpp"

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
//void ProgressiveIndex::node_refinement(QSAVLTree* tree, QSAVLNode* node, IdxColEntry* column, ssize_t& remaining_swaps) {
//    if (!node) {
//        return;
//    }
//    //! Check if node has children
//    if (node->noChildren()) {
//        auto pieceStart = tree->pieceStart(node);
//        auto pieceEnd = tree->pieceEnd(node);
//        if ((pieceEnd->offset - pieceStart->offset) <= 1024) { //! small enough to sort?
//            //! node is very small, just sort it normally
//            if (remaining_swaps > (pieceEnd->offset - pieceStart->offset) * 5) {
//                hybrid_radixsort_insert(column + pieceStart->offset, pieceEnd->offset - pieceStart->offset + 1);
//                node->sorted = true;
//                //! Check if there are nodes we should merge
//                auto parent = tree->findParent(node);
//                remaining_swaps -= (pieceEnd->offset - pieceStart->offset) * 5; //! log2(8192)
//                tree->mergeChildren(parent);
//            }
//            return;
//        }
//        //! does it have a pivot going on?
//        while (node->current_start < node->current_end && remaining_swaps > 0) {
//            //! TODO: we could scan while we pivot here
//            auto start = column[node->current_start];
//            auto end = column[node->current_end];
//
//            int start_has_to_swap = start >= node->pivot;
//            int end_has_to_swap = end < node->pivot;
//            int has_to_swap = start_has_to_swap * end_has_to_swap;
//
//            column[node->current_start].m_key = !has_to_swap * start.m_key + has_to_swap * end.m_key;
//            column[node->current_end].m_key = !has_to_swap * end.m_key + has_to_swap * start.m_key;
//            column[node->current_start].m_rowId = !has_to_swap * start.m_rowId + has_to_swap * end.m_rowId;
//            column[node->current_end].m_rowId = !has_to_swap * end.m_rowId + has_to_swap * start.m_rowId;
//
//            node->current_start += !start_has_to_swap + has_to_swap;
//            node->current_end -= !end_has_to_swap + has_to_swap;
//
//            remaining_swaps--;
//        }
//
//        //! We finish refining this node's pivot
//        if (node->current_start >= node->current_end && !node->sorted) {
//            //! Time to procreate
//            //! construct the left and right side of the root node
//            auto pieceStart = tree->pieceStart(node);
//            auto pieceEnd = tree->pieceEnd(node);
//            if ((pieceEnd->offset - pieceStart->offset) <= 1024) { //! small enough to sort?
//                //! node is very small, just sort it normally
//                if (remaining_swaps > (pieceEnd->offset - pieceStart->offset) * 5) {
//                    hybrid_radixsort_insert(column + pieceStart->offset, pieceEnd->offset - pieceStart->offset);
//                    node->sorted = true;
//                    //! Check if there are nodes we should merge
//                    auto parent = tree->findParent(node);
//                    remaining_swaps -= (pieceEnd->offset - pieceStart->offset) * 5; //! log2(8192)
//                    tree->mergeChildren(parent);
//                }
//                return;
//            }
//            //! Left Node
//            size_t current_start = pieceStart->offset;
//            size_t current_end = node->current_end;
//            int64_t pivot = (pieceStart->value + node->pivot) / 2;
//            assert(pivot < node->pivot);
//            node->setLeft(make_unique<QSAVLNode>(pivot, current_start, current_end));
//
//            //! Right node
//            current_start = current_end;
//            current_end = pieceEnd->offset;
//            pivot = (pieceEnd->value + node->pivot) / 2;
//            assert(pivot > node->pivot);
//            node->setRight(make_unique<QSAVLNode>(pivot, current_start, current_end));
//        }
//        return;
//    } else {
//        //! Node has children, visit them
//        auto left = node->left.get();
//        //! node has children, go into one of the children
//        node_refinement(tree, left, column, remaining_swaps);
//        auto right = node->right.get();
//        node_refinement(tree, right, column, remaining_swaps);
//    }
//}
//ResultStruct ProgressiveIndex::progressive_quicksort_refine(ProgressiveIndex& progressiveIndex, ssize_t& remaining_swaps, int64_t low, int64_t high) {
//    auto indexColumn = progressiveIndex.column->data;
//    auto tree = progressiveIndex.tree.get();
//    int64_t low_cop = low;
//    int64_t high_cop = high;
//    //! Get Boundaries for query
//    auto lowNode = progressiveIndex.tree->FindNodeGTE(low);
//    //! if lowNode is null we get first node
//    if (!lowNode) {
//        lowNode = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
//    }
//    //! Prioritize a bit of cracking on the boundary nodes
//    node_refinement(tree, lowNode, indexColumn, remaining_swaps);
//    auto highNode = progressiveIndex.tree->FindNodeLT(high);
//    if (!highNode) {
//        highNode = lowNode;
//    }
//    node_refinement(tree, highNode, indexColumn, remaining_swaps);
//
//    //! If we still have budget after finishing the priority cracking we keep on cracking
//    auto node = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
//    while (remaining_swaps > 1024 * 5 && !tree->root->sorted) {
//        if (!node) {
//            node = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
//        }
//        if (node->sorted || !node->noChildren()) {
//            //! 47864
//            node = progressiveIndex.tree->inOrderSucessor(node);
//            continue;
//        }
//        node_refinement(tree, node, indexColumn, remaining_swaps);
//        node = progressiveIndex.tree->inOrderSucessor(node);
//    }
//    assert(remaining_swaps <= 1024 * 5 || tree->root->sorted);
//    auto results = refinement_scan(tree, indexColumn, low_cop, high_cop, progressiveIndex.column->size);
//    if (isTest) {
//        check_column(indexColumn, progressiveIndex.column->size);
//        int64_t result = full_scan(indexColumn, progressiveIndex.column->size, low_cop, high_cop);
//        assert(results.sum == result);
//    }
//    return results;
//}

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
            up.maybe_push_back(i,matching);
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
        int64_t pivot = indexColumn[root->current_start/2];
        size_t current_start = 0;
        size_t current_end = root->current_end;

        root->setLeft(make_unique<KDNode>(pivot, current_start, current_end));

        //! Right node
        pivot = indexColumn[(root->current_start+table_size)/2];
        current_start = current_end + 1;
        current_end = table_size - 1;
        root->setRight(make_unique<KDNode>(pivot, current_start, current_end));
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
        assert(0 && "Refinement Not Yet Implemented");
//        progressive_quicksort_refine(query, remaining_swaps);
    }
    //! We are in the consolidation phase no more indexing to be done, just scan it.
    assert(0);
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
