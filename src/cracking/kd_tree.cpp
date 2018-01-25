#include "kd_tree.h"
#include <algorithm>

int64_t THRESHOLD = 100;

struct KDNode
{
    ElementType element;
    int64_t column;

    KDNode *left;  // less than element
    KDNode *right; // greater or equal than element

    int64_t left_position;
    int64_t right_position;
};

void exchange(Table &t, int64_t x1, int64_t x2)
{
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
int64_t CrackTable(Table table, int64_t low, int64_t high, ElementType element, int64_t c)
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

KDTree CreateNode(int64_t column, ElementType element, int64_t left_position, int64_t right_position)
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

KDTree CheckLeftSide(KDTree current, int64_t column, ElementType element, int64_t lower_limit, int64_t upper_limit, Table table)
{
    if (current->left == NULL)
    {
        int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

        if (position < lower_limit)
        {
            current->left = CreateNode(column, element, -1, lower_limit);
        }
        else
        {
            current->left = CreateNode(column, element, position, position + 1);
        }
        return NULL;
    }
    else
    {
        return current->left;
    }
}

KDTree CheckRightSide(KDTree current, int64_t column, ElementType element, int64_t lower_limit, int64_t upper_limit, Table table)
{
    if (current->right == NULL)
    {
        int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

        if (position < lower_limit)
        {
            current->right = CreateNode(column, element, -1, lower_limit);
        }
        else
        {
            current->right = CreateNode(column, element, position, position + 1);
        }
        return NULL;
    }
    else
    {
        return current->right;
    }
}

void InsertIntoRoot(KDTree &tree, int64_t column, ElementType element, Table table)
{
    int64_t lower_limit = 0;
    int64_t upper_limit = table.ids.size() - 1;
    int64_t position = CrackTable(table, lower_limit, upper_limit, element, column);

    if (position < lower_limit)
    {
        tree = CreateNode(column, element, -1, 0);
    }
    else
    {
        tree = CreateNode(column, element, position, position + 1);
    }
}

void Insert(KDTree &tree, int64_t column, ElementType element, Table table)
{
    if (tree == NULL)
    {
        return InsertIntoRoot(tree, column, element, table);
    }

    std::vector<KDTree> nodes_to_check;
    std::vector<int64_t> lower_limits, upper_limits;

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
                upper_limit = current->left_position;
                KDTree new_node = CheckLeftSide(current, column, element, lower_limit, upper_limit, table);
                if (new_node != NULL)
                {
                    nodes_to_check.push_back(new_node);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(upper_limit);
                }
            }
            else
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
        else
        {
            KDTree new_node = CheckLeftSide(current, column, element, lower_limit, upper_limit, table);
            if (new_node != NULL)
            {
                nodes_to_check.push_back(new_node);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(upper_limit);
            }

            new_node = CheckRightSide(current, column, element, lower_limit, upper_limit, table);
            if (new_node != NULL)
            {
                nodes_to_check.push_back(new_node);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(upper_limit);
            }
        }
    }
}

std::vector<int64_t> collect_results(Table table, int64_t lower_limit, int64_t upper_limit, std::vector<std::pair<int64_t, int64_t>> query)
{
    std::vector<int64_t> result;
    for (size_t line = lower_limit; line <= upper_limit; ++line)
    {
        bool valid = true;
        for (size_t col = 0; col < table.columns.size() && valid; col++)
        {
            int64_t e = table.columns.at(col).at(line);
            if (!(query.at(col).first <= e && e < query.at(col).second))
            {
                valid = false;
            }
        }
        if (valid)
            result.push_back(table.ids.at(line));
    }
    return result;
}

std::vector<int64_t> SearchKDTree(KDTree &tree, std::vector<std::pair<int64_t, int64_t>> query, Table table, bool should_crack = false)
{
    std::vector<int64_t> ids;

    if (should_crack)
    {
        for (size_t col = 0; col < query.size(); ++col)
        {
            int64_t leftKey = query.at(col).first;
            int64_t rightKey = query.at(col).second;

            Insert(tree, col, leftKey, table);
            Insert(tree, col, rightKey, table);
        }
    }

    std::vector<KDTree> nodes_to_check;
    std::vector<int64_t> lower_limits, upper_limits;

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
            if (current->left == NULL)
            {
                std::vector<int64_t> partial = collect_results(table, lower_limit, current->left_position, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->left);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(current->left_position);
            }
        }
        else if (query.at(current->column).first >= current->element)
        {
            if (current->right == NULL)
            {
                std::vector<int64_t> partial = collect_results(table, current->right_position, upper_limit, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->right);
                lower_limits.push_back(current->right_position);
                upper_limits.push_back(upper_limit);
            }
        }
        else
        {
            if (current->left == NULL)
            {
                std::vector<int64_t> partial = collect_results(table, lower_limit, current->left_position, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->left);
                lower_limits.push_back(lower_limit);
                upper_limits.push_back(current->left_position);
            }
            if (current->right == NULL)
            {
                std::vector<int64_t> partial = collect_results(table, current->right_position, upper_limit, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->right);
                lower_limits.push_back(current->right_position);
                upper_limits.push_back(upper_limit);
            }
        }
    }

    return ids;
}

// int64_t find_median(std::vector<Row> lines, int64_t column)
// {
//     std::vector<int64_t> elements(lines.size());
//     for (size_t i = 0; i < lines.size(); i++)
//     {
//         elements.at(i) = lines.at(i).data.at(column);
//     }

//     std::nth_element(elements.begin(), elements.begin() + elements.size() / 2, elements.end());

//     return elements[elements.size() / 2];
// }

// KDTree FullKDTree(std::vector<Row> lines)
// {
//     int n_of_cols = lines.at(0).data.size();
//     std::vector<std::pair<KDTree, int64_t>> nodes;

//     KDTree index = CreateNode(0, find_median(lines, 0), lines);

//     nodes.push_back(std::make_pair(index, 0));

//     while (!nodes.empty())
//     {
//         KDTree current = nodes.back().first;
//         int64_t column = (nodes.back().second + 1) % n_of_cols;
//         nodes.pop_back();

//         if (current->left_rows.size() > THRESHOLD)
//         {
//             current->left = CreateNode(column, find_median(current->left_rows, column), current->left_rows);
//             current->left_rows = std::vector<Row>();

//             nodes.push_back(std::make_pair(current->left, column));
//         }

//         if (current->right_rows.size() > THRESHOLD)
//         {
//             current->right = CreateNode(column, find_median(current->right_rows, column), current->right_rows);
//             current->right_rows = std::vector<Row>();

//             nodes.push_back(std::make_pair(current->right, column));
//         }
//     }

//     return index;
// }

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