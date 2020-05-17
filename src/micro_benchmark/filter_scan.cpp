#include <cstdio>
#include <chrono>
#include <math.h>
#include <generators/uniform_generator.hpp>
#include <candidate_list.hpp>
#include <bitvector.hpp>
#include <bitvector.h>

using namespace std;
using namespace chrono;

//! Full_Scan with candidate list new
void fs_cln(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
        CandidateList cl;
        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                cl.push_back(c_idx);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            CandidateList new_cl(cl.size);
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t cl_idx = 0; cl_idx < cl.size; cl_idx++) {
                if (column[cl.get(cl_idx)] >= low && column[cl.get(cl_idx)] <= high) {
                    new_cl.push_back(cl.get(cl_idx));
                }
            }
            //! Move new_cl -> cl
            cl.initialize(new_cl);
        }
        //! Iterate through final cl to get sum
        column = table.columns[0]->data;
        for (size_t cl_idx = 0; cl_idx < cl.size; cl_idx++) {
            sum += column[cl.get(cl_idx)];
        }
    }
}

//! Full_Scan with candidate list replace
void fs_cls(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
        CandidateList cl;
        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                cl.push_back(c_idx);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            vector<int> not_valid;
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t cl_idx = 0; cl_idx < cl.size; cl_idx++) {
                if (column[cl.get(cl_idx)] >= low && column[cl.get(cl_idx)] <= high) {
//                    new_cl.push_back(cl.get(cl_idx));
                }
            }
            //! Move new_cl -> cl
//            cl.initialize(new_cl);
        }
        //! Iterate through final cl to get sum
        column = table.columns[0]->data;
        for (size_t cl_idx = 0; cl_idx < cl.size; cl_idx++) {
            sum += column[cl.get(cl_idx)];
        }
    }
}

//! Full_Scan with bitvector + get
void fs_bvg(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
//        auto start_timer = system_clock::now();
        BitVector bv = BitVector(table.row_count());
//        auto end_timer = system_clock::now();
//        auto init_time = duration<double>(end_timer - start_timer).count();
//        cout << "Bit Vector (init) with get : " << init_time << endl;
        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                bv.set(c_idx, 1);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
                if (bv.get(c_idx)) {
                    if (column[c_idx] < low || column[c_idx] > high) {
                        bv.set(c_idx, 0);
                    }
                }
            }
        }
        //! Iterate through final bv to get sum
        column = table.columns[0]->data;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (bv.get(c_idx)) {
                sum += column[c_idx];
            }
        }
    }
}

//! Full_Scan with bitvector (Storm) + get
void fs_bvgs(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
//        auto start_timer = system_clock::now();
//        uint_fast64_t bv_size = table.row_count();
        storm::storage::BitVector bv = storm::storage::BitVector(table.row_count());
//        auto end_timer = system_clock::now();
//        auto init_time = duration<double>(end_timer - start_timer).count();
//        cout << "Bit Vector (init) with get : " << init_time << endl;
        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                bv.set(c_idx, 1);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
                if (bv.get(c_idx)) {
                    if (column[c_idx] < low || column[c_idx] > high) {
                        bv.set(c_idx, 0);
                    }
                }
            }
        }
        //! Iterate through final bv to get sum
        column = table.columns[0]->data;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (bv.get(c_idx)) {
                sum += column[c_idx];
            }
        }
    }
}

//! Full_Scan with bitvector + AND
void fs_bva(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
        BitVector bv = BitVector(table.row_count());
        BitVector bv_aux = BitVector(table.row_count());

        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                bv.set(c_idx, 1);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
                bv_aux.set(c_idx, column[c_idx] < low || column[c_idx] > high);
            }
            bv.bitwise_and(bv_aux);
        }
        //! Iterate through final bv to get sum
        column = table.columns[0]->data;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (bv.get(c_idx)) {
                sum += column[c_idx];
            }
        }
    }
}

//! Full_Scan with bitvector + AND
void fs_bvas(Table &table, Workload &workload, size_t dimensions, double &sum) {
    for (size_t w_idx = 0; w_idx < workload.query_count(); w_idx++) {
         storm::storage::BitVector bv =  storm::storage::BitVector(table.row_count());
         storm::storage::BitVector bv_aux =  storm::storage::BitVector(table.row_count());

        //! Create cl based on first column
        auto column = table.columns[0]->data;
        auto low = workload.queries[w_idx].predicates[0].low;
        auto high = workload.queries[w_idx].predicates[0].high;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (column[c_idx] >= low && column[c_idx] <= high) {
                bv.set(c_idx, 1);
            }
        }
        for (size_t d_idx = 1; d_idx < dimensions; d_idx++) {
            column = table.columns[d_idx]->data;
            low = workload.queries[w_idx].predicates[d_idx].low;
            high = workload.queries[w_idx].predicates[d_idx].high;
            for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
                bv_aux.set(c_idx, column[c_idx] < low || column[c_idx] > high);
            }
            bv&=bv_aux;
        }
        //! Iterate through final bv to get sum
        column = table.columns[0]->data;
        for (size_t c_idx = 0; c_idx < table.row_count(); c_idx++) {
            if (bv.get(c_idx)) {
                sum += column[c_idx];
            }
        }
    }
}

int main(int argc, char **argv) {
    size_t n_of_rows = pow(10, 7);
    size_t dimensions = 2;
    //! Note that selectivity is per query, not per column
    float selectivity = 0.001; //! 1 = 100%
    size_t number_of_queries = 10;
    string DATA_FILE = "/home/holanda/Projects/MultidimensionalAdaptiveIndexing/data";
    string QUERY_FILE = "/home/holanda/Projects/MultidimensionalAdaptiveIndexing/query";
    double cln_sum = 0;
    //! Generate Table + Queries
    auto generator = UniformGenerator(n_of_rows, dimensions, selectivity, number_of_queries);
    generator.generate(DATA_FILE, QUERY_FILE);
    auto table = Table::read_file(DATA_FILE);
    auto workload = Workload::read_file(QUERY_FILE);


    auto start_timer = system_clock::now();
    fs_cln(*table, workload, dimensions, cln_sum);
    auto end_timer = system_clock::now();
    auto scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << cln_sum << endl;
    cout << "Candidate List with new : " << scan_time << endl;


    start_timer = system_clock::now();
    fs_cls(*table, workload, dimensions, cln_sum);
    end_timer = system_clock::now();
    scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << cln_sum << endl;
    cout << "Candidate List with swap : " << scan_time << endl;

    double bvg_sum = 0;
    start_timer = system_clock::now();
    fs_bvg(*table, workload, dimensions, bvg_sum);
    end_timer = system_clock::now();
    scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << bvg_sum << endl;
    cout << "Bit Vector with get : " << scan_time << endl;

    double bvgs_sum = 0;
    start_timer = system_clock::now();
    fs_bvgs(*table, workload, dimensions, bvgs_sum);
    end_timer = system_clock::now();
    scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << bvgs_sum << endl;
    cout << "Bit Vector (Storm) with get : " << scan_time << endl;

    double bvn_sum = 0;
    start_timer = system_clock::now();
    fs_bva(*table, workload, dimensions, bvn_sum);
    end_timer = system_clock::now();
    scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << bvn_sum << endl;
    cout << "Bit Vector with new : " << scan_time << endl;

    double bvsn_sum = 0;
    start_timer = system_clock::now();
    fs_bvas(*table, workload, dimensions, bvsn_sum);
    end_timer = system_clock::now();
    scan_time = duration<double>(end_timer - start_timer).count() / number_of_queries;
    cout << bvsn_sum << endl;
    cout << "Bit Vector (Storm) with new : " << scan_time << endl;
}
