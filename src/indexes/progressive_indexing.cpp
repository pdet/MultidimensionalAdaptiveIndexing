#include <chrono>
#include <iostream>
#include "progressive_index.hpp"

using namespace std;
using namespace chrono;

bool isTest = false;

unique_ptr<RefinementScan> ProgressiveIndex::find_offsets(QSAVLTree* tree,IdxColEntry* column, int64_t low, int64_t high, size_t col_size){
    auto offsets = make_unique<RefinementScan>();
    //! Find left side
    auto leftNode = tree->FindNodeGTE(low);
    //! Find Right side
    auto rightNode = tree->FindNodeLT(high);
    if (leftNode) {
        if (leftNode->sorted) {
            auto pieceStart = tree->pieceStart(leftNode);
            auto pieceEnd = tree->pieceEnd(leftNode);
            offsets->offsetLeft = binary_search_gte(column, low, pieceStart->offset, pieceEnd->offset);
            offsets->checkLeft = false;
        } else {
            auto pieceEnd = tree->pieceEnd(leftNode);
            offsets-> offsetLeft = leftNode->current_start;
            offsets->offsetLeftMiddle = pieceEnd->offset;
        }
    } else {
        offsets->offsetLeft = 0;
        auto firstPiece = tree->FindMin(tree->root.get());
        offsets->offsetLeftMiddle = firstPiece->current_end;
    }
    if (rightNode) {
        if (rightNode->sorted) {
            offsets->checkRight = false;
            auto pieceStart = tree->pieceStart(rightNode);
            auto pieceEnd = tree->pieceEnd(rightNode);
            offsets->offsetRight = binary_search_lte(column, high, pieceStart->offset, pieceEnd->offset);
        } else {
            auto pieceStart = tree->pieceStart(rightNode);
            offsets->offsetRightMiddle = pieceStart->offset;
            offsets->offsetRight = rightNode->current_end;
        }
    } else {
        offsets->offsetRight = col_size - 1;
        auto lastPiece = tree->FindMax(tree->root.get());
        offsets->offsetRightMiddle = lastPiece->current_start;
    }
    return move(offsets);

}

ResultStruct ProgressiveIndex::refinement_scan(int64_t low, int64_t high, size_t col_size) {
    ResultStruct result;
    auto offsets = find_offsets(tree,column,low,high,col_size);
    //! Both pieces are sorted
    if (!offsets->checkLeft && !offsets->checkRight) {
        for (size_t i = offsets->offsetLeft; i < offsets->offsetRight; i++) {
            result.push_back(column[i]);
        }
    } else if (!offsets->checkLeft) { //! Only Left is Sorted
        for (size_t i = offsets->offsetLeft; i < offsets->offsetRightMiddle; i++) {
            result.push_back(column[i]);
        }
        //! We check the values of the right node
        for (size_t i = offsets->offsetRightMiddle; i <= offsets->offsetRight; i++) {
            int match = low <= column[i].m_key && column[i] < high;
            result.maybe_push_back(column[i], match);
        }
    } else if (!offsets->checkRight) { //! Only right is sorted
        for (size_t i = offsets->offsetLeft; i < offsets->offsetLeftMiddle; i++) {
            int match = low <= column[i].m_key && column[i] < high;
            result.maybe_push_back(column[i], match);
        }
        for (size_t i = offsets->offsetLeftMiddle; i < offsets->offsetRight; i++) {
            result.push_back(column[i]);
        }
    } else { //! No sorted nodes
        //! check if there are no middle pieces
        if (offsets->offsetLeft == offsets->offsetRightMiddle || offsets->offsetLeftMiddle == offsets->offsetRight) {
            for (size_t i = offsets->offsetLeft; i <= offsets->offsetLeftMiddle; i++) {
                int match = low <= column[i].m_key && column[i] < high;
                result.maybe_push_back(column[i], match);
            }
        } else { //! We have middle pieces
            //! We only have one middle piece that has not finished pivoting
            if (offsets->offsetLeftMiddle >= offsets->offsetRightMiddle) {
                //! We have to match everything
                for (size_t i = offsets->offsetLeft; i <= offsets->offsetRight; i++) {
                    int match = low <= column[i].m_key && column[i] < high;
                    result.maybe_push_back(column[i], match);
                }
            } else {
                for (size_t i = offsets->offsetLeft; i <= offsets->offsetLeftMiddle; i++) {
                    int match = low <= column[i].m_key && column[i] < high;
                    result.maybe_push_back(column[i], match);
                }
                //! No need to match middle pieces
                for (size_t i = offsets->offsetLeftMiddle + 1; i < offsets->offsetRightMiddle; i++) {
                    result.push_back(column[i]);
                }
                for (size_t i = offsets->offsetRightMiddle; i <= offsets->offsetRight; i++) {
                    int match = low <= column[i].m_key && column[i] < high;
                    result.maybe_push_back(column[i], match);
                }
            }
        }
    }

    return result;
}
void ProgressiveIndex::node_refinement(QSAVLTree* tree, QSAVLNode* node, IdxColEntry* column, ssize_t& remaining_swaps) {
    if (!node) {
        return;
    }
    //! Check if node has children
    if (node->noChildren()) {
        auto pieceStart = tree->pieceStart(node);
        auto pieceEnd = tree->pieceEnd(node);
        if ((pieceEnd->offset - pieceStart->offset) <= 1024) { //! small enough to sort?
            //! node is very small, just sort it normally
            if (remaining_swaps > (pieceEnd->offset - pieceStart->offset) * 5) {
                hybrid_radixsort_insert(column + pieceStart->offset, pieceEnd->offset - pieceStart->offset + 1);
                node->sorted = true;
                //! Check if there are nodes we should merge
                auto parent = tree->findParent(node);
                remaining_swaps -= (pieceEnd->offset - pieceStart->offset) * 5; //! log2(8192)
                tree->mergeChildren(parent);
            }
            return;
        }
        //! does it have a pivot going on?
        while (node->current_start < node->current_end && remaining_swaps > 0) {
            //! TODO: we could scan while we pivot here
            auto start = column[node->current_start];
            auto end = column[node->current_end];

            int start_has_to_swap = start >= node->pivot;
            int end_has_to_swap = end < node->pivot;
            int has_to_swap = start_has_to_swap * end_has_to_swap;

            column[node->current_start].m_key = !has_to_swap * start.m_key + has_to_swap * end.m_key;
            column[node->current_end].m_key = !has_to_swap * end.m_key + has_to_swap * start.m_key;
            column[node->current_start].m_rowId = !has_to_swap * start.m_rowId + has_to_swap * end.m_rowId;
            column[node->current_end].m_rowId = !has_to_swap * end.m_rowId + has_to_swap * start.m_rowId;

            node->current_start += !start_has_to_swap + has_to_swap;
            node->current_end -= !end_has_to_swap + has_to_swap;

            remaining_swaps--;
        }

        //! We finish refining this node's pivot
        if (node->current_start >= node->current_end && !node->sorted) {
            //! Time to procreate
            //! construct the left and right side of the root node
            auto pieceStart = tree->pieceStart(node);
            auto pieceEnd = tree->pieceEnd(node);
            if ((pieceEnd->offset - pieceStart->offset) <= 1024) { //! small enough to sort?
                //! node is very small, just sort it normally
                if (remaining_swaps > (pieceEnd->offset - pieceStart->offset) * 5) {
                    hybrid_radixsort_insert(column + pieceStart->offset, pieceEnd->offset - pieceStart->offset);
                    node->sorted = true;
                    //! Check if there are nodes we should merge
                    auto parent = tree->findParent(node);
                    remaining_swaps -= (pieceEnd->offset - pieceStart->offset) * 5; //! log2(8192)
                    tree->mergeChildren(parent);
                }
                return;
            }
            //! Left Node
            size_t current_start = pieceStart->offset;
            size_t current_end = node->current_end;
            int64_t pivot = (pieceStart->value + node->pivot) / 2;
            assert(pivot < node->pivot);
            node->setLeft(make_unique<QSAVLNode>(pivot, current_start, current_end));

            //! Right node
            current_start = current_end;
            current_end = pieceEnd->offset;
            pivot = (pieceEnd->value + node->pivot) / 2;
            assert(pivot > node->pivot);
            node->setRight(make_unique<QSAVLNode>(pivot, current_start, current_end));
        }
        return;
    } else {
        //! Node has children, visit them
        auto left = node->left.get();
        //! node has children, go into one of the children
        node_refinement(tree, left, column, remaining_swaps);
        auto right = node->right.get();
        node_refinement(tree, right, column, remaining_swaps);
    }
}
ResultStruct ProgressiveIndex::progressive_quicksort_refine(ProgressiveIndex& progressiveIndex, ssize_t& remaining_swaps, int64_t low, int64_t high) {
    auto indexColumn = progressiveIndex.column->data;
    auto tree = progressiveIndex.tree.get();
    int64_t low_cop = low;
    int64_t high_cop = high;
    //! Get Boundaries for query
    auto lowNode = progressiveIndex.tree->FindNodeGTE(low);
    //! if lowNode is null we get first node
    if (!lowNode) {
        lowNode = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
    }
    //! Prioritize a bit of cracking on the boundary nodes
    node_refinement(tree, lowNode, indexColumn, remaining_swaps);
    auto highNode = progressiveIndex.tree->FindNodeLT(high);
    if (!highNode) {
        highNode = lowNode;
    }
    node_refinement(tree, highNode, indexColumn, remaining_swaps);

    //! If we still have budget after finishing the priority cracking we keep on cracking
    auto node = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
    while (remaining_swaps > 1024 * 5 && !tree->root->sorted) {
        if (!node) {
            node = progressiveIndex.tree->FindMin(progressiveIndex.tree->root.get());
        }
        if (node->sorted || !node->noChildren()) {
            //! 47864
            node = progressiveIndex.tree->inOrderSucessor(node);
            continue;
        }
        node_refinement(tree, node, indexColumn, remaining_swaps);
        node = progressiveIndex.tree->inOrderSucessor(node);
    }
    assert(remaining_swaps <= 1024 * 5 || tree->root->sorted);
    auto results = refinement_scan(tree, indexColumn, low_cop, high_cop, progressiveIndex.column->size);
    if (isTest) {
        check_column(indexColumn, progressiveIndex.column->size);
        int64_t result = full_scan(indexColumn, progressiveIndex.column->size, low_cop, high_cop);
        assert(results.sum == result);
    }
    return results;
}

ResultStruct ProgressiveIndex::progressive_quicksort_create(vector<int64_t>& column, ProgressiveIndex& progressiveIndex, ssize_t& remaining_swaps, int64_t low,
                                          int64_t high) {
    ResultStruct results;
    auto indexColumn = progressiveIndex.column->data;
    auto root = (QSAVLNode*)progressiveIndex.tree->root.get();
    //! for the initial run, we write the indices instead of swapping them
    //! because the current array has not been initialized yet
    //! first look through the part we have already pivoted
    //! for data that matches the points
    if (low <= root->pivot) {
        for (size_t i = 0; i < root->current_start; i++) {
            int matching = indexColumn[i] >= low && indexColumn[i] < high;
            results.maybe_push_back(indexColumn[i], matching);
        }
    }
    if (high >= root->pivot) {
        for (size_t i = root->current_end + 1; i < column.size(); i++) {
            int matching = indexColumn[i] >= low && indexColumn[i] < high;
            results.maybe_push_back(indexColumn[i], matching);
        }
    }

    //! now we start filling the index with at most remaining_swap entries
    size_t next_index = min(progressiveIndex.current_position + remaining_swaps, column.size());
    remaining_swaps -= next_index - progressiveIndex.current_position;
    for (size_t i = progressiveIndex.current_position; i < next_index; i++) {
        int matching = column[i] >= low && column[i] < high;
        results.maybe_push_back(column[i], matching);

        int bigger_pivot = column[i] >= root->pivot;
        int smaller_pivot = 1 - bigger_pivot;

        indexColumn[root->current_start].m_key = column[i];
        indexColumn[root->current_start].m_rowId = i;
        indexColumn[root->current_end].m_key = column[i];
        indexColumn[root->current_end].m_rowId = i;

        root->current_start += smaller_pivot;
        root->current_end -= bigger_pivot;
    }
    progressiveIndex.current_position = next_index;

    //! Check if we are finished with the initial run
    if (next_index == column.size()) {
        assert(root->current_start >= root->current_end);
        //! construct the left and right side of the root node
        int64_t pivot = progressiveIndex.column->size / 4;
        size_t current_start = 0;
        size_t current_end = root->current_end;

        root->setLeft(make_unique<QSAVLNode>(pivot, current_start, current_end));

        //! Right node
        pivot = (progressiveIndex.column->size / 4) * 3;
        current_start = current_end + 1;
        current_end = progressiveIndex.column->size - 1;
        root->setRight(make_unique<QSAVLNode>(pivot, current_start, current_end));
    } else {
        //! we have done all the swapping for this run
        //! now we query the remainder of the data
        for (size_t i = progressiveIndex.current_position; i < column.size(); i++) {
            int matching = column[i] >= low && column[i] < high;
            results.maybe_push_back(column[i], matching);
        }
    }
    return results;
}

void ProgressiveIndex::progressive_quicksort(Table *originalTable,Query& query) {
    //! Amount of work we are allowed to do
    ssize_t remaining_swaps = (ssize_t)(table->row_count() * delta);

    //! Creation Phase
    //! If the node has no children we are stil in the creation phase
    assert(tree->root);
    if (tree->root->noChildren()) {
        //! Creation Phase
        //! TODO: We might still have swaps left to do after the  creation phase Need to implement a smooth transition
        progressive_quicksort_create(originalTable, query, remaining_swaps);
    } else if (!tree->root->sorted) { //! If the root is not marked as sort we still have refinement to do!
        //! Refinement Phase
        progressive_quicksort_refine(query, remaining_swaps);
    }
    //! We are in the consolidation phase no more indexing to be done.
}

void ProgressiveIndex::progressive_scan(Table *originalTable,Query& query) {
  assert(tree->root);
  if (tree->root->noChildren()) {
    //! Creation Phase Scan
    progressive_quicksort_create(originalTable, query);
  } else {
    //! Regular Scan
    progressive_quicksort_refine(query);
  }
}


double ProgressiveIndex::get_costmodel_delta_quicksort(vector<int64_t>& originalColumn, int64_t low, int64_t high, double delta) {
return 0.0;
}


void ProgressiveIndex::initialize(Table *table_to_copy){
  auto start = measurements->time();
  //! FIXME: Guarantee that pivot is avg
  table = make_unique<IdxTbl>(table_to_copy->col_count(),table_to_copy->row_count());
  initializeRoot(table_to_copy->row_count()/2,table_to_copy->row_count());
  auto end = measurements->time();
  measurements->append(
      "initialization_time",
      std::to_string(Measurements::difference(end, start))
  );
}

void ProgressiveIndex::adapt_index(Table *originalTable,Query& query){
  // FIXME: Before adapting calculate the scan overhead to measure how much the previous
  // queries helped this one
//  auto partitions = index->search(query);
//  n_tuples_scanned_before_adapting = 0;
//  for(auto &partition : partitions)
//    n_tuples_scanned_before_adapting += partition.second - partition.first;


  // Transform query into points and edges before starting to measure time
  // Adapt the KDTree
  auto start = measurements->time();
  progressive_quicksort(originalTable,query);
  auto end = measurements->time();
  // ******************
  measurements->append(
      "adaptation_time",
      std::to_string(Measurements::difference(end, start))
  );

}

Table ProgressiveIndex::range_query(Table *originalTable,Query& query){
  // ******************
  auto start = measurements->time();

  // Search on the index the correct partitions
  auto partitions = index->search(query);

  auto end = measurements->time();
  measurements->append(
      "index_search_time",
      std::to_string(Measurements::difference(end, start))
  );

  start = measurements->time();
  // Scan the table and returns the row ids
  auto result = Table(1);
  for (auto partition : partitions)
  {
    auto low = partition.first;
    auto high = partition.second;
    FullScan::scan_partition(table.get(), query, low, high, &result);
  }

  end = measurements->time();
  // ******************
  measurements->append(
      "scan_time",
      std::to_string(Measurements::difference(end, start))
  );

  int64_t n_tuples_scanned = 0;
  for(auto &partition : partitions)
    n_tuples_scanned += partition.second - partition.first;

  // Before returning the result, update the statistics.
  measurements->append("number_of_nodes", std::to_string(index->get_node_count()));
  measurements->append("max_height", std::to_string(index->get_max_height()));
  measurements->append("min_height", std::to_string(index->get_min_height()));
  measurements->append("memory_footprint", std::to_string(index->get_node_count() * sizeof(KDNode)));
  measurements->append("tuples_scanned", std::to_string(n_tuples_scanned));

  measurements->append(
      "scan_overhead_before_adapt",
      std::to_string(
          n_tuples_scanned_before_adapting/static_cast<float>(result.row_count())
      )
  );

  measurements->append(
      "scan_overhead_after_adapt",
      std::to_string(
          n_tuples_scanned/static_cast<float>(result.row_count())
      )
  );

  return result;
}
