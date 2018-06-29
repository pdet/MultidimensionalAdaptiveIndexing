#include "kd_tree.h"
// #include "structs.h"
#include <algorithm>

extern int64_t KDTREE_THRESHOLD, COLUMN_SIZE, NUMBER_OF_COLUMNS;
using namespace std;


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

Tree CreateNode(int64_t column, int64_t element, int64_t left_position, int64_t right_position)
{
    Tree node = new Node();
    node->column = column;
    node->Element = element;
    node->Left = NULL;
    node->Right = NULL;
    node->left_position = left_position;
    node->right_position = right_position;
    return node;
}

Tree CheckLeftSide(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit, Table &table)
{
    if (current->Left == NULL)
    {
        if (upper_limit - lower_limit + 1 > KDTREE_THRESHOLD)
        {
            int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->Left = CreateNode(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->Left = CreateNode(column, element, upper_limit, -1);
            }
            else
            {
                current->Left = CreateNode(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->Left;
    }
}

Tree CheckRightSide(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit, Table &table)
{
    if (current->Right == NULL)
    {
        if (upper_limit - lower_limit + 1 > KDTREE_THRESHOLD)
        {
            int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->Right = CreateNode(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->Right = CreateNode(column, element, upper_limit, -1);
            }
            else
            {
                current->Right = CreateNode(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->Right;
    }
}

void InsertIntoRoot(Tree &tree, int64_t column, int64_t element, Table &table)
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

void Insert(Tree &tree, int64_t column, int64_t element, Table &table)
{
    if (tree == NULL)
    {
        return InsertIntoRoot(tree, column, element, table);
    }

    vector<Tree> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    lower_limits.push_back(0);
    upper_limits.push_back(table.ids.size() - 1);
    nodes_to_check.push_back(tree);

    while (!nodes_to_check.empty())
    {
        Tree current = nodes_to_check.back();
        nodes_to_check.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if (current->column == column)
        {
            if (current->Element == element)
                continue;
            else if (element < current->Element)
            {
                if (current->left_position != -1)
                {
                    upper_limit = current->left_position;
                    Tree new_node = CheckLeftSide(current, column, element, lower_limit, upper_limit, table);
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
                    Tree new_node = CheckRightSide(current, column, element, lower_limit, upper_limit, table);
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
                Tree new_node = CheckLeftSide(current, column, element, lower_limit, current->left_position, table);
                if (new_node != NULL)
                {
                    nodes_to_check.push_back(new_node);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                Tree new_node = CheckRightSide(current, column, element, current->right_position, upper_limit, table);
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

    int sel_size;
    int sel_vector[upper_limit - lower_limit];

    int64_t result = 0;

    sel_size = select_rq_scan_new (sel_vector, &table.columns[0][lower_limit],query.at(0).first,query.at(0).second,upper_limit - lower_limit);
        for (int column_num = 1; column_num < table.columns.size(); column_num++){
            sel_size = select_rq_scan_sel_vec(sel_vector, &table.columns[column_num][lower_limit],query.at(column_num).first,query.at(column_num).second,sel_size);
        }
        result += sel_size;
    return result;
}


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



Tree FullTree(Table &table)
{
    int n_of_cols = table.columns.size();
    int col_size = table.ids.size() - 1;
    vector<Tree> nodes;
    vector<int64_t> columns;
    vector<int64_t> lower_limits;
    vector<int64_t> upper_limits;

    pair<int64_t, int64_t> median_result;
    median_result = find_median(table, 0, 0, col_size);
    int64_t median = median_result.first;
    int64_t p = median_result.second;
    Tree tree;

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
        Tree current = nodes.back();
        nodes.pop_back();

        int64_t column = (columns.back() + 1) % n_of_cols;
        columns.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        if (current->left_position != -1)
        {
            if ((current->left_position - lower_limit + 1) > KDTREE_THRESHOLD)
            {
                median_result = find_median(table, column, lower_limit, current->left_position);
                int64_t element = median_result.first;
                int64_t position = median_result.second;

                if (position < lower_limit)
                {
                    current->Left = CreateNode(column, element, -1, lower_limit);
                }
                else if (position >= current->left_position)
                {
                    current->Left = CreateNode(column, element, current->left_position, -1);
                }
                else
                {
                    current->Left = CreateNode(column, element, position, position + 1);

                    nodes.push_back(current->Left);
                    columns.push_back(column);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
        }

        if (current->right_position != -1)
        {
            if ((upper_limit - current->right_position + 1) > KDTREE_THRESHOLD)
            {
                median_result = find_median(table, column, current->right_position, upper_limit);
                int64_t element = median_result.first;
                int64_t position = median_result.second;

                if (position < current->right_position)
                {
                    current->Right = CreateNode(column, element, -1, current->right_position);
                }
                else if (position >= upper_limit)
                {
                    current->Right = CreateNode(column, element, upper_limit, -1);
                }
                else
                {
                    current->Right = CreateNode(column, element, position, position + 1);

                    nodes.push_back(current->Right);
                    columns.push_back(column);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
    }

    return tree;
}


void freeTree(Tree tree)
{
    if (tree == NULL)
        return;

    if (tree->Left != NULL)
    {
        freeTree(tree->Left);
    }

    if (tree->Right != NULL)
    {
        freeTree(tree->Right);
    }
    delete tree;
}

void Print(Tree T)
{
    if (T == NULL)
        return;
    printf("(%lld,%lld) ", (long long int)T->Element, (long long int)T->Element);
    printf("\n");
    Print(T->Right);
    Print(T->Left);
}

void cracking_kdtree_pre_processing(Column *c,Table table, Tree * T){
    table.columns = vector<vector<int64_t>>(NUMBER_OF_COLUMNS);
    table.ids = vector<int64_t>(COLUMN_SIZE);
    for (size_t col = 0; col < NUMBER_OF_COLUMNS; ++col)
    {
        table.columns.at(col) = vector<int64_t>(COLUMN_SIZE);
        for (size_t line = 0; line < COLUMN_SIZE; ++line)
        {
            table.ids.at(line) = line;
            table.columns.at(col).at(line) = c[col].data[line];
        }
    }
    T = NULL;
}

void cracking_kdtree_partial_built(IndexEntry **crackercolumns, Tree * T,Table table,vector<pair<int64_t,int64_t>>  *rangequeries){
    for (size_t i = 0; i < rangequeries->size(); ++i){
        Insert(*T, i, rangequeries->at(i).first, table);
        Insert(*T, i, rangequeries->at(i).second, table);
    }
}


// void cracking_kdtree_index_lookup(Tree * tree,vector<pair<int64_t,int64_t>>  *query, Table &table,vector<pair<int,int>>  *offsets)
// {
//     // fprintf(stderr, "Query: %ld -- %ld\n", query.at(0).first, query.at(0).second);
//     vector<Tree> nodes_to_check;
//     vector<int64_t> lower_limits, upper_limits;

//     lower_limits.push_back(0);
//     upper_limits.push_back(table.ids.size() - 1);
//     nodes_to_check.push_back(tree);
//     while (!nodes_to_check.empty())
//     {
//         Tree current = nodes_to_check.back();
//         nodes_to_check.pop_back();

//         int64_t lower_limit = lower_limits.back();
//         lower_limits.pop_back();

//         int64_t upper_limit = upper_limits.back();
//         upper_limits.pop_back();

//         if (query.at(current->column).second <= current->element)
//         {
//             if (current->left_position != -1)
//             {
//                 if (current->Left == NULL)
//                 {
//                     offsets->push_back(make_pair(lower_limit, current->left_position));
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->Left);
//                     lower_limits.push_back(lower_limit);
//                     upper_limits.push_back(current->left_position);
//                 }
//             }
//         }
//         else if (query.at(current->column).first >= current->element)
//         {
//             if (current->right_position != -1)
//             {
//                 if (current->Right == NULL)
//                 {
//                     offsets->push_back(make_pair(current->right_position, upper_limit));
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->Right);
//                     lower_limits.push_back(current->right_position);
//                     upper_limits.push_back(upper_limit);
//                 }
//             }
//         }
//         else
//         {
//             if (current->left_position != -1)
//             {
//                 if (current->Left == NULL)
//                 {
//                     offsets->push_back(make_pair(current->right_position, upper_limit));
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->Left);
//                     lower_limits.push_back(lower_limit);
//                     upper_limits.push_back(current->left_position);
//                 }
//             }
//             if (current->right_position != -1)
//             {
//                 if (current->Right == NULL)
//                 {
//                     offsets->push_back(make_pair(current->right_position, upper_limit));
//                 }
//                 else
//                 {
//                     nodes_to_check.push_back(current->Right);
//                     lower_limits.push_back(current->right_position);
//                     upper_limits.push_back(upper_limit);
//                 }
//             }
//         }
//     }
// }

