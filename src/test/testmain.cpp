int64_t range_query_baseline(Column *c, RangeQuery *queries, size_t query_index)
{
	int64_t result = 0;
	for (size_t i = 0; i <= COLUMN_SIZE - 1; ++i)
	{
		bool is_valid = true;
		for (size_t j = 0; j < NUMBER_OF_COLUMNS && is_valid; ++j)
		{
			int64_t keyL = queries[j].leftpredicate[query_index];
			int64_t keyH = queries[j].rightpredicate[query_index];
			if (!(c[j].data[i] >= keyL && c[j].data[i] < keyH))
			{
				is_valid = false;
			}
		}
		if (is_valid)
		{
			result += c[0].data[i];
		}
	}
	return result;
}

bool verify_range_query(Column *c, RangeQuery *queries, size_t query_index, int64_t received)
{
	int64_t result = range_query_baseline(c, queries, query_index);
	if (received != result)
	{
		fprintf(stderr, "Incorrect Results!\n");
		fprintf(stderr, "Expected: %ld\n", result);
		fprintf(stderr, "Got: %ld\n", received);
		assert(0);
		return false;
	}
	return true;
}

bool verify_range_query(Column& c, range_query_function function, double delta) {
    c.Clear();
    srand(1234);
    for(int i = 0; i <= 1000; i++) {
        if (i % 100 == 0) printf("Iteration %d/1000\n", i);
        int l = c.data[rand() % c.data.size()];
        int r = c.data[rand() % c.data.size()];
        if (l > r) {
            std::swap(l, r);
        }
        auto r1 = range_query_baseline(c.data, l, r);

        ResultStruct r2;
        if (c.converged) {
            range_query_sorted_subsequent_value(c.final_data, c.data.size(), l, r, r2);
        } else {
            r2 = function(c, l, r, delta);
        }
        std::sort(r1.begin(), r1.end());
        std::sort(r2.begin(), r2.end());
        if (r1.size() != r2.size() ||
            !std::equal(r1.begin(), r1.end(), r2.begin())) {
            printf("Incorrect results in Iteration %d!  \n", i);
            for(size_t i = 0; i < r1.size() && i < r2.size(); i++) {
                if (r1[i] != r2[i]) {
                    printf("Difference in element %zu [%lld != %lld]", i, (int64_t)r1[i], (int64_t)r2[i]);
                    break;
                }
            }
            assert(0);
            return false;
        }
    }
    return true;
}

void verifyAlgorithms(Column& c){
    double delta = 0.1;
    printf("Running predicated mergesort.\n");
    verify_range_query(c, range_query_incremental_mergesort, delta);
    printf("Running branched mergesort.\n");
    verify_range_query(c, range_query_incremental_mergesort_branched, delta);
    printf("Running predicated quicksort.\n");
    verify_range_query(c, range_query_incremental_quicksort, delta);
    printf("Running branched quicksort.\n");
    verify_range_query(c, range_query_incremental_quicksort_branched, delta);
    printf("Running predicated bucketsort (equi-width).\n");
    verify_range_query(c, range_query_incremental_bucketsort_equiwidth, delta);
    printf("Running branched bucketsort (equi-width).\n");
    verify_range_query(c, range_query_incremental_bucketsort_equiwidth_branched, delta);
    printf("Running predicated bucketsort (equi-height).\n");
    verify_range_query(c, range_query_incremental_bucketsort_equiheight, delta);
    printf("Running branched bucketsort (equi-height).\n");
    verify_range_query(c, range_query_incremental_bucketsort_equiheight_branched, delta);
    printf("Running predicated radixsort.\n");
    verify_range_query(c, range_query_incremental_radixsort, delta);
    printf("Running branched radixsort.\n");
    verify_range_query(c, range_query_incremental_radixsort_branched, delta);
}
