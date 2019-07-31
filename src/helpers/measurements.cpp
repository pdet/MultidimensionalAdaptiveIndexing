#include <chrono>
#include <vector>
#include "measurements.hpp"
#include <iostream>

Measurements::Measurements(){}
Measurements::~Measurements(){}

Measurements::time_point Measurements::time(){
    return std::chrono::steady_clock::now();
}

double Measurements::difference(Measurements::time_point end, Measurements::time_point start){
    return std::chrono::duration_cast<std::chrono::microseconds>(end - start).count()/1000000.0;
}

void Measurements::save_to_sql(std::string db_name, int repetition, std::string alg_name){
    rc = sqlite3_open(db_name.c_str(), &db);

    if( rc ) {
        std::cout << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    create_table();

    insert(repetition, alg_name);

    sqlite3_close(db);
}

void Measurements::create_table(){
    /* Create SQL statement */
   auto sql = "CREATE TABLE IF NOT EXISTS RESULTS("  \
      "NAME                     TEXT        NOT NULL," \
      "INITIALIZATION_TIME      FLOAT       NOT NULL," \
      "ADAPTATION_TIME          FLOAT       NOT NULL," \
      "QUERY_TIME               FLOAT       NOT NULL," \
      "MIN_HEIGHT               INT         NOT NULL," \
      "MAX_HEIGHT               INT         NOT NULL," \
      "NUMBER_OF_NODES          INT         NOT NULL," \
      "MEMORY_FOOTPRINT         INT         NOT NULL," \
      "REPETITION               INT         NOT NULL" \
      ");";

    // std::cout << sql << std::endl;

   /* Execute SQL statement */
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);

   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }
}

void Measurements::insert(size_t repetition, const std::string alg_name){
    for(size_t i = 0; i < adaptation_time.size(); ++i){
        auto sql = "INSERT INTO RESULTS "\
                   "(NAME, INITIALIZATION_TIME, ADAPTATION_TIME, QUERY_TIME, MIN_HEIGHT, MAX_HEIGHT, NUMBER_OF_NODES, MEMORY_FOOTPRINT, REPETITION) "\
                   "VALUES "\
                   "(" + quotes(alg_name) + ", " \
                   + quotes(std::to_string(initialization_time)) + ", " \
                   + quotes(std::to_string(adaptation_time.at(i))) + ", " \
                   + quotes(std::to_string(query_time.at(i))) + ", " \
                   + quotes(std::to_string(min_height.at(i))) + ", " \
                   + quotes(std::to_string(max_height.at(i))) + ", " \
                   + quotes(std::to_string(number_of_nodes.at(i))) + ", " \
                   + quotes(std::to_string(memory_footprint.at(i))) + ", " \
                   + quotes(std::to_string(repetition)) +\
                   + ")";

        // std::cout << sql << std::endl;

        rc = sqlite3_exec(db, sql.c_str(), callback, 0, &zErrMsg);

        if( rc != SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
    }
}

std::string Measurements::quotes(std::string s){
    return "'" + s + "'";
}

int Measurements::callback(void *NotUsed, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}