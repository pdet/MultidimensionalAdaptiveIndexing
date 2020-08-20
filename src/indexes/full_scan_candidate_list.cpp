#include "full_scan_candidate_list.hpp"
#include <cassert>
#include <vector>

using namespace std;

FullScanCandidateList::FullScanCandidateList(
    std::map<std::string, std::string> /*config*/) {}
FullScanCandidateList::~FullScanCandidateList() {}

void FullScanCandidateList::initialize(Table *table_to_copy) {

  // Simply copies the pointer of the table, since it does not change anything
  table = make_unique<Table>(table_to_copy);

  // FIXME: should just copy the pointer, instead of copying the entire table.
  //          for now it can be left like this and make init time equal 0
  auto start = measurements->time();
  auto end = measurements->time();

  measurements->append("initialization_time",
                       std::to_string(Measurements::difference(end, start)));
}

void FullScanCandidateList::adapt_index(Query &) {
  // Zero adaptation for full scan
  measurements->append("adaptation_time",
                       std::to_string(Measurements::difference(
                           measurements->time(), measurements->time())));
}

//std::pair<double, size_t> full_scan(Table *t, Query &query) {
//  auto dim = 0;
//  auto low = query.predicates[dim].low;
//  auto high = query.predicates[dim].high;
//  auto column = t->columns[dim]->data;
//  CandidateList list;
//  for (size_t i = 0; i < t->row_count(); i++) {
//    int matching = column[i] >= low && column[i] <= high;
//    list.maybe_push_back(i, matching);
//  }
//  for (dim = 1; dim < t->col_count(); dim++) {
//    if (list.size == 0) {
//      break;
//    }
//    auto qualifying_index = 0;
//    low = query.predicates[dim].low;
//    high = query.predicates[dim].high;
//    column = t->columns[dim]->data;
//    for (size_t i = 0; i < list.size; i++) {
//      int matching = column[list.get(i)] >= low && column[list.get(i)] <= high;
//      list.data[qualifying_index] = list.get(i);
//      qualifying_index += matching;
//    }
//                list.size = qualifying_index;
//
//  }
//  double sum = 0;
//  column = t->columns[0]->data;
//  for (size_t i = 0; i < list.size; i ++){
//    sum += column[list.get(i)];
//  }
//    return std::make_pair(sum, list.size);
//}

unique_ptr<Table> FullScanCandidateList::range_query(Query &query) {

    // Scan the table and returns a materialized view of the result.
    std::vector<std::pair<size_t, size_t>> partitions;
    partitions.push_back(std::make_pair(0, table->row_count()));
    std::vector<std::vector<bool>> per_partition_attribute_skip(partitions.size(), std::vector<bool>(table->col_count(), false));
    auto start = measurements->time();
    auto result = FullScanCandidateList::scan_partition(
            table.get(), query, partitions, per_partition_attribute_skip
    );
    auto end = measurements->time();

    measurements->append("scan_time",
                         std::to_string(Measurements::difference(end, start)));

    // Before returning the result, update the statistics.
    measurements->append("tuples_scanned", std::to_string(table->row_count()));

    measurements->append(
            "scan_overhead",
            std::to_string(table->row_count() / static_cast<float>(result.second)));

    auto t = make_unique<Table>(2);
    float row[2] = {static_cast<float>(result.first),
                    static_cast<float>(result.second)};
    t->append(row);
    return t;
}
