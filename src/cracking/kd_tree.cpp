#include "kd_tree.h"
// #include "structs.h"
#include "../util/timer.h"
#include <algorithm>
#include <chrono>
int64_t THRESHOLD = 100;
using namespace std;

struct KDNode
{
    int64_t element;
    int64_t column;

    KDNode *left;  // less than element
    KDNode *right; // greater or equal than element

    int64_t left_position;
    int64_t right_position;
};

void configKDTree(int64_t threshold)
{
    THRESHOLD = threshold;
    if (THRESHOLD < 1)
    {
        THRESHOLD = 1;
    }
}

void exchange(Table &t, int64_t x1, int64_t x2)
{
    if (x1 == x2)
        return;
    int64_t tmp;
    tmp = t.ids.at(x1);
    t.ids.at(x1) = t.ids.at(x2);
    t.ids.at(x2) = tmp;

    for (size_t i = 0; i < t.columns.size(); i++)
    {
        tmp = t.columns.at(i).at(x1);
        t.columns.at(i).at(x1) = t.columns.at(i).at(x2);
        t.columns.at(i).at(x2) = tmp;
    }
}

// Cracks table from position i = low_ until i == high_, on determined column with the element
int64_t CrackTable(Table &table, int64_t low, int64_t high, int64_t element, int64_t c)
{
    int64_t x1 = low;
    int64_t x2 = high;

    while (x1 <= x2)
    {
        if (table.columns.at(c).at(x1) < element)
            x1++;
        else
        {
            while (x2 >= x1 && (table.columns.at(c).at(x2) >= element))
                x2--;
            if (x1 < x2)
            {
                exchange(table, x1, x2);
                x1++;
                x2--;
            }
        }
    }
    if (x1 < x2)
        printf("Not all elements were inspected!");
    x1--;
    return x1;
}

KDTree CreateNode(int64_t column, int64_t element, int64_t left_position, int64_t right_position)
{
    KDTree node = new KDNode();
    node->column = column;
    node->element = element;
    node->left = NULL;
    node->right = NULL;
    node->left_position = left_position;
    node->right_position = right_position;
    return node;
}

KDTree CheckLeftSide(KDTree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit, Table &table)
{
    if (current->left == NULL)
    {
        if (upper_limit - lower_limit + 1 > THRESHOLD)
        {
            int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->left = CreateNode(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->left = CreateNode(column, element, upper_limit, -1);
            }
            else
            {
                current->left = CreateNode(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->left;
    }
}

KDTree CheckRightSide(KDTree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit, Table &table)
{
    if (current->right == NULL)
    {
        if (upper_limit - lower_limit + 1 > THRESHOLD)
        {
            int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->right = CreateNode(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->right = CreateNode(column, element, upper_limit, -1);
            }
            else
            {
                current->right = CreateNode(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->right;
    }
}

void InsertIntoRoot(KDTree &tree, int64_t column, int64_t element, Table &table)
{
    int64_t lower_limit = 0;
    int64_t upper_limit = table.ids.size() - 1;
    int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

    if (position < lower_limit)
    {
        tree = CreateNode(column, element, -1, 0);
    }
    else if (position >= upper_limit)
    {
        tree = CreateNode(column, element, upper_limit, -1);
    }
    else
    {
        tree = CreateNode(column, element, position, position + 1);
    }
}

void Insert(KDTree &tree, int64_t column, int64_t element, Table &table)
{
    if (tree == NULL)
    {
        return InsertIntoRoot(tree, column, element, table);
    }

    vector<KDTree> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    lower_limits.push_back(0);
    upper_limits.push_back(table.ids.size() - 1);
    nodes_to_check.push_back(tree);

    while (!nodes_to_check.empty())
    {
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if (current->column == column)
        {
            if (current->element == element)
                continue;
            else if (element < current->element)
            {
                if (current->left_position != -1)
                {
                    upper_limit = current->left_position;
                    KDTree new_node = CheckLeftSide(current, column, element, lower_limit, upper_limit, table);
                    if (new_node != NULL)
                    {
                        nodes_to_check.push_back(new_node);
                        lower_limits.push_back(lower_limit);
                        upper_limits.push_back(upper_limit);
                    }
                }
            }
            else
            {
                if (current->right_position != -1)
                {
                    lower_limit = current->right_position;
                    KDTree new_node = CheckRightSide(current, column, element, lower_limit, upper_limit, table);
                    if (new_node != NULL)
                    {
                        nodes_to_check.push_back(new_node);
                        lower_limits.push_back(lower_limit);
                        upper_limits.push_back(upper_limit);
                    }
                }
            }
        }
        else
        {
            if (current->left_position != -1)
            {
                KDTree new_node = CheckLeftSide(current, column, element, lower_limit, current->left_position, table);
                if (new_node != NULL)
                {
                    nodes_to_check.push_back(new_node);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                KDTree new_node = CheckRightSide(current, column, element, current->right_position, upper_limit, table);
                if (new_node != NULL)
                {
                    nodes_to_check.push_back(new_node);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
    }
}


int64_t collect_results(Table &table, int64_t lower_limit, int64_t upper_limit, vector<pair<int64_t, int64_t>> query, size_t currentQueryNum)
{
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    int sel_size;
    int sel_vector[upper_limit - lower_limit];

    int64_t result = 0;

    sel_size = select_rq_scan_new (sel_vector, &table.columns[0][lower_limit],query.at(0).first,query.at(0).second,upper_limit - lower_limit);
        for (int column_num = 1; column_num < table.columns.size(); column_num++){
            sel_size = select_rq_scan_sel_vec(sel_vector, &table.columns[column_num][lower_limit],query.at(column_num).first,query.at(column_num).second,sel_size);
        }
        result += sel_size;
    
    end = chrono::system_clock::now();
    scanTime.at(currentQueryNum) += chrono::duration<double>(end - start).count();
    return result;
}

int64_t SearchKDTree(KDTree &tree, vector<pair<int64_t, int64_t>> query, Table &table, bool should_crack = false, size_t currentQueryNum = 0)
{
    chrono::time_point<chrono::system_clock> start, end;
    int64_t result = 0;

    if (should_crack)
    {
        start = chrono::system_clock::now();
        for (size_t col = 0; col < query.size(); ++col)
        {
            int64_t leftKey = query.at(col).first;
            int64_t rightKey = query.at(col).second;

            Insert(tree, col, leftKey, table);
            Insert(tree, col, rightKey, table);
        }
        end = chrono::system_clock::now();
        indexCreation.at(currentQueryNum) = chrono::duration<double>(end - start).count();
    }

    start = chrono::system_clock::now();

    // fprintf(stderr, "Query: %ld -- %ld\n", query.at(0).first, query.at(0).second);

    vector<KDTree> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    lower_limits.push_back(0);
    upper_limits.push_back(table.ids.size() - 1);
    nodes_to_check.push_back(tree);
    while (!nodes_to_check.empty())
    {
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if (query.at(current->column).second <= current->element)
        {
            if (current->left_position != -1)
            {
                if (current->left == NULL)
                {
                    result += collect_results(table, lower_limit, current->left_position, query, currentQueryNum);
                }
                else
                {
                    nodes_to_check.push_back(current->left);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
        }
        else if (query.at(current->column).first >= current->element)
        {
            if (current->right_position != -1)
            {
                if (current->right == NULL)
                {
                    result += collect_results(table, current->right_position, upper_limit, query, currentQueryNum);
                }
                else
                {
                    nodes_to_check.push_back(current->right);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
        else
        {
            if (current->left_position != -1)
            {
                if (current->left == NULL)
                {
                    result += collect_results(table, lower_limit, current->left_position, query, currentQueryNum);
                }
                else
                {
                    nodes_to_check.push_back(current->left);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                if (current->right == NULL)
                {
                    result += collect_results(table, current->right_position, upper_limit, query, currentQueryNum);
                }
                else
                {
                    nodes_to_check.push_back(current->right);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
    }
    end = chrono::system_clock::now();
    indexLookup.at(currentQueryNum) = chrono::duration<double>(end - start).count() - scanTime.at(currentQueryNum);

    return result;
}

// vector<int64_t> SearchKDTreeProgressive(KDTree &tree, vector<pair<int64_t, int64_t>> query, Table &table, bool should_crack = false, size_t currentQueryNum = 0)
// {
//     chrono::time_point<chrono::system_clock> start, end;
//     vector<int64_t> ids;
//     int col_size = table.ids.size() - 1;
//     if (should_crack)
//     {
//         start = chrono::system_clock::now();
//         for (size_t col = 0; col < query.size(); ++col)
//         {
// //            Choosing random key for now.
//             int64_t key = rand() % col_size;
//             Insert(tree, col, key, table);
//         }
//         end = chrono::system_clock::now();
// //        indexCreation.at(currentQueryNum) = chrono::duration<double>(end - start).count();
//     }

//     start = chrono::system_clock::now();

//     // fprintf(stderr, "Query: %ld -- %ld\n", query.at(0).first, query.at(0).second);

//     vector<KDTree> nodes_to_check;
//     vector<int64_t> lower_limits, upper_limits;

//     lower_limits.push_back(0);
//     upper_limits.push_back(table.ids.size() - 1);
//     nodes_to_check.push_back(tree);
//     while (!nodes_to_check.empty())
//     {
//         KDTree current = nodes_to_check.back();
//         nodes_to_check.pop_back();

//         int64_t lower_limit = lower_limits.back();
//         lower_limits.pop_back();

//         int64_t upper_limit = upper_limits.back();
//         upper_limits.pop_back();

//         if (query.at(current->column).second <= current->element)
//         {
//             if (current->left_position != -1)
//             {
//                 if (current->left == NULL)
//                 {
//                     vector<int64_t> partial = collect_results(table, lower_limit, current->left_position, query, currentQueryNum);
//                     ids.insert(ids.end(), partial.begin(), partial.end());
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->left);
//                     lower_limits.push_back(lower_limit);
//                     upper_limits.push_back(current->left_position);
//                 }
//             }
//         }
//         else if (query.at(current->column).first >= current->element)
//         {
//             if (current->right_position != -1)
//             {
//                 if (current->right == NULL)
//                 {
//                     vector<int64_t> partial = collect_results(table, current->right_position, upper_limit, query, currentQueryNum);
//                     ids.insert(ids.end(), partial.begin(), partial.end());
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->right);
//                     lower_limits.push_back(current->right_position);
//                     upper_limits.push_back(upper_limit);
//                 }
//             }
//         }
//         else
//         {
//             if (current->left_position != -1)
//             {
//                 if (current->left == NULL)
//                 {
//                     vector<int64_t> partial = collect_results(table, lower_limit, current->left_position, query, currentQueryNum);
//                     ids.insert(ids.end(), partial.begin(), partial.end());
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->left);
//                     lower_limits.push_back(lower_limit);
//                     upper_limits.push_back(current->left_position);
//                 }
//             }
//             if (current->right_position != -1)
//             {
//                 if (current->right == NULL)
//                 {
//                     vector<int64_t> partial = collect_results(table, current->right_position, upper_limit, query, currentQueryNum);
//                     ids.insert(ids.end(), partial.begin(), partial.end());
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->right);
//                     lower_limits.push_back(current->right_position);
//                     upper_limits.push_back(upper_limit);
//                 }
//             }
//         }
//         end = chrono::system_clock::now();
// //        indexLookup.at(currentQueryNum) = chrono::duration<double>(end - start).count() - scanTime.at(currentQueryNum);
//     }

//     return ids;
// }

// This method only works if we use the last element as the pivot
int pivot_table(Table &table, int64_t column, int64_t low, int64_t high, int64_t pivot)
{
    int64_t i = low - 1;

    for (int64_t j = low; j < high; ++j)
    {
        if (table.columns.at(column).at(j) <= pivot)
        {
            ++i;
            exchange(table, i, j);
        }
    }
    exchange(table, i + 1, high);
    return i + 1;
}

pair<int64_t, int64_t> find_median(Table &table, int64_t column, int64_t lower_limit, int64_t upper_limit)
{
    int64_t low = lower_limit;
    int64_t high = upper_limit;
    int64_t position, element;

    do
    {
        element = table.columns.at(column).at(high);
        position = pivot_table(table, column, low, high, element);

        if (position <= low)
        {
            ++low;
        }
        else if (position >= high)
        {
            --high;
        }
        else
        {
            if (position < (lower_limit + upper_limit) / 2)
                low = position;
            else
                high = position;
        }
    } while (position != (lower_limit + upper_limit) / 2);

    for (; position > lower_limit; --position)
    {
        if (table.columns.at(column).at(position - 1) != table.columns.at(column).at(position))
            break;
    }

    return make_pair(element, position - 1);
}



KDTree FullKDTree(Table &table)
{
    int n_of_cols = table.columns.size();
    int col_size = table.ids.size() - 1;
    vector<KDTree> nodes;
    vector<int64_t> columns;
    vector<int64_t> lower_limits;
    vector<int64_t> upper_limits;

    pair<int64_t, int64_t> median_result;
    median_result = find_median(table, 0, 0, col_size);
    int64_t median = median_result.first;
    int64_t p = median_result.second;
    KDTree tree;

    if (p < 0)
    {
        tree = CreateNode(0, median, -1, 0);
    }
    else if (p >= col_size)
    {
        tree = CreateNode(0, median, col_size, -1);
    }
    else
    {
        tree = CreateNode(0, median, p, p + 1);
    }

    nodes.push_back(tree);
    columns.push_back(0);
    lower_limits.push_back(0);
    upper_limits.push_back(col_size);

    while (!nodes.empty())
    {
        KDTree current = nodes.back();
        nodes.pop_back();

        int64_t column = (columns.back() + 1) % n_of_cols;
        columns.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if (current->left_position != -1)
        {
            if ((current->left_position - lower_limit + 1) > THRESHOLD)
            {
                median_result = find_median(table, column, lower_limit, current->left_position);
                int64_t element = median_result.first;
                int64_t position = median_result.second;

                if (position < lower_limit)
                {
                    current->left = CreateNode(column, element, -1, lower_limit);
                }
                else if (position >= current->left_position)
                {
                    current->left = CreateNode(column, element, current->left_position, -1);
                }
                else
                {
                    current->left = CreateNode(column, element, position, position + 1);

                    nodes.push_back(current->left);
                    columns.push_back(column);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
        }

        if (current->right_position != -1)
        {
            if ((upper_limit - current->right_position + 1) > THRESHOLD)
            {
                median_result = find_median(table, column, current->right_position, upper_limit);
                int64_t element = median_result.first;
                int64_t position = median_result.second;

                if (position < current->right_position)
                {
                    current->right = CreateNode(column, element, -1, current->right_position);
                }
                else if (position >= upper_limit)
                {
                    current->right = CreateNode(column, element, upper_limit, -1);
                }
                else
                {
                    current->right = CreateNode(column, element, position, position + 1);

                    nodes.push_back(current->right);
                    columns.push_back(column);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
    }

    return tree;
}


void freeKDTree(KDTree tree)
{
    if (tree == NULL)
        return;

    if (tree->left != NULL)
    {
        freeKDTree(tree->left);
    }

    if (tree->right != NULL)
    {
        freeKDTree(tree->right);
    }
    delete tree;
}

void Print(KDTree T)
{
    if (T == NULL)
        return;
    printf("(%lld,%lld) ", (long long int)T->element, (long long int)T->element);
    printf("\n");
    Print(T->right);
    Print(T->left);
}
