#include <cstdlib>
#include <iostream>
#include "../src/util/structs.h"
#include "../src/util/file_manager.h"
std::string COLUMN_FILE_PATH, QUERIES_FILE_PATH;
int64_t COLUMN_SIZE, NUMBER_OF_COLUMNS, NUM_QUERIES;

void generateColumnCSV() {
    FILE* f = fopen("./column.csv", "w+");
    Column *c = (Column *)malloc(sizeof(Column) * NUMBER_OF_COLUMNS);
	loadcolumn(c,COLUMN_FILE_PATH, COLUMN_SIZE, NUMBER_OF_COLUMNS);
	for (size_t line = 0; line < COLUMN_SIZE; ++line) {
		for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col) {
			fprintf(f,"%ld",c[col].data[line]);
			if(col != NUMBER_OF_COLUMNS-1)
				fprintf(f,";");
			else
				fprintf(f, "\n");
		}
	}
	fclose(f);
}


void generateQuerySQL() {
    FILE* f = fopen("./query.sql", "w+");
    RangeQuery *rangequeries = (RangeQuery *)malloc(sizeof(RangeQuery) * NUMBER_OF_COLUMNS);
	loadQueries(rangequeries, QUERIES_FILE_PATH, NUM_QUERIES, NUMBER_OF_COLUMNS);
	
	for (size_t query_index = 0; query_index < NUM_QUERIES; ++query_index) {
		fprintf(f,"select count(*) from t where  ");
		for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col) {
			fprintf(f,"c%zu between %ld and %ld",col,rangequeries[col].leftpredicate[query_index],rangequeries[col].rightpredicate[query_index]);
		if(col != NUMBER_OF_COLUMNS-1)
			fprintf(f," and ");
		else
			fprintf(f, ";\n");

		}
	}
	fclose(f);
}

int main(int argc, char const *argv[]) {
	COLUMN_FILE_PATH = "/export/scratch1/home/holanda/MultidimensionalAdaptiveIndexing/column.txt";
	QUERIES_FILE_PATH = "/export/scratch1/home/holanda/MultidimensionalAdaptiveIndexing/query.txt";
	COLUMN_SIZE = 10000000;
	NUMBER_OF_COLUMNS = 16;
	NUM_QUERIES = 1000;
	generateColumnCSV();
	generateQuerySQL();
	return 0;
}