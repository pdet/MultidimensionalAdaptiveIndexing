#include "kd_tree.h"
#include <algorithm>
#include "structs.h"

int64_t THRESHOLD = 100;

struct KDNode
{
    ElementType element;
    int64_t column;

    KDNode *left;  // less than element
    KDNode *right; // greater or equal than element

    std::vector<Row> left_rows;
    std::vector<Row> right_rows;
};

KDTree CreateNode(int64_t column, ElementType element, std::vector<Row> lines)
{
    KDTree node = new KDNode();
    node->column = column;
    node->element = element;
    node->left = NULL;
    node->right = NULL;

    for (size_t i = 0; i < lines.size(); ++i)
    {
        if (lines.at(i).data.at(column) < element)
            node->left_rows.push_back(lines.at(i));
        else
            node->right_rows.push_back(lines.at(i));
    }
    return node;
}

KDTree CheckLeftSide(KDTree current, int64_t column, ElementType element)
{
    if (current->left == NULL)
    {
        if (current->left_rows.size() > THRESHOLD)
        {
            current->left = CreateNode(column, element, current->left_rows);
            current->left_rows = std::vector<Row>();
        }
        return NULL;
    }
    else
    {
        return current->left;
    }
}

KDTree CheckRightSide(KDTree current, int64_t column, ElementType element)
{
    if (current->right == NULL)
    {
        if (current->right_rows.size() > THRESHOLD)
        {
            current->right = CreateNode(column, element, current->right_rows);
            current->right_rows = std::vector<Row>();
        }
        return NULL;
    }
    else
    {
        return current->right;
    }
}

KDTree Insert(KDTree tree, int64_t column, ElementType element, std::vector<Row> lines)
{
    if (tree == NULL)
    {
        return CreateNode(column, element, lines);
    }

    std::vector<KDTree> nodes_to_check;
    nodes_to_check.push_back(tree);
    while (!nodes_to_check.empty())
    {
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back(); // delete the last element

        if (current->column == column)
        {
            if (current->element == element)
            {
                continue;
            }
            else if (element < current->element)
            {
                KDTree next_node = CheckLeftSide(current, column, element);
                if (next_node != NULL)
                    nodes_to_check.push_back(next_node);
            }
            else
            {
                KDTree next_node = CheckRightSide(current, column, element);
                if (next_node != NULL)
                    nodes_to_check.push_back(next_node);
            }
        }
        else
        {
            KDTree next_node = CheckLeftSide(current, column, element);
            if (next_node != NULL)
                nodes_to_check.push_back(next_node);

            next_node = CheckRightSide(current, column, element);
            if (next_node != NULL)
                nodes_to_check.push_back(next_node);
        }
    }
    return tree;
}

std::vector<int64_t> collect_results(std::vector<Row> partial, std::vector<std::pair<int64_t, int64_t>> query)
{
    std::vector<int64_t> result;
    for (size_t i = 0; i < partial.size(); ++i)
    {
        bool valid = true;
        for (size_t j = 0; j < partial.at(i).data.size() && valid; j++)
        {
            int64_t e = partial.at(i).data.at(j);
            if (!(query.at(j).first <= e && e < query.at(j).second))
            {
                valid = false;
            }
        }
        if (valid)
            result.push_back(partial.at(i).id);
    }
    return result;
}

std::vector<int64_t> SearchKDTree(KDTree &index, std::vector<std::pair<int64_t, int64_t>> query, std::vector<Row> lines, bool should_crack = false)
{
    std::vector<int64_t> ids;

    if (should_crack)
    {
        for (size_t col = 0; col < query.size(); ++col)
        {
            int64_t lowKey = query.at(col).first;
            int64_t highKey = query.at(col).second;

            index = Insert(index, col, lowKey, lines);
            index = Insert(index, col, highKey, lines);
        }
    }

    std::vector<KDTree> nodes_to_check;
    nodes_to_check.push_back(index);
    while (!nodes_to_check.empty())
    {
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back(); // delete the last element

        if (query.at(current->column).second <= current->element)
        {
            if (current->left == NULL)
            {
                std::vector<int64_t> partial = collect_results(current->left_rows, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->left);
            }
        }
        else if (query.at(current->column).first >= current->element)
        {
            if (current->right == NULL)
            {
                std::vector<int64_t> partial = collect_results(current->right_rows, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->right);
            }
        }
        else
        {
            if (current->left == NULL)
            {
                std::vector<int64_t> partial = collect_results(current->left_rows, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->left);
            }

            if (current->right == NULL)
            {
                std::vector<int64_t> partial = collect_results(current->right_rows, query);
                ids.insert(ids.end(), partial.begin(), partial.end());
            }
            else
            {
                nodes_to_check.push_back(current->right);
            }
        }
    }

    return ids;
}

int64_t find_median(std::vector<Row> lines, int64_t column)
{
    std::vector<int64_t> elements(lines.size());
    for (size_t i = 0; i < lines.size(); i++)
    {
        elements.at(i) = lines.at(i).data.at(column);
    }

    std::nth_element(elements.begin(), elements.begin() + elements.size() / 2, elements.end());

    return elements[elements.size() / 2];
}

KDTree FullKDTree(std::vector<Row> lines)
{
    int n_of_cols = lines.at(0).data.size();
    std::vector<std::pair<KDTree, int64_t>> nodes;

    KDTree index = CreateNode(0, find_median(lines, 0), lines);

    nodes.push_back(std::make_pair(index, 0));

    while (!nodes.empty())
    {
        KDTree current = nodes.back().first;
        int64_t column = (nodes.back().second + 1) % n_of_cols;
        nodes.pop_back();

        if (current->left_rows.size() > THRESHOLD)
        {
            current->left = CreateNode(column, find_median(current->left_rows, column), current->left_rows);
            current->left_rows = std::vector<Row>();

            nodes.push_back(std::make_pair(current->left, column));
        }

        if (current->right_rows.size() > THRESHOLD)
        {
            current->right = CreateNode(column, find_median(current->right_rows, column), current->right_rows);
            current->right_rows = std::vector<Row>();

            nodes.push_back(std::make_pair(current->right, column));
        }
    }

    return index;
}

//TODO Adapt to progressive copy
//TODO Check if random access is worst than copy the data
//TODO Predication
//TODO Add Delta
void PatialKDTree(std::vector<std::pair<KDTree, int64_t>> *nodes, int n_of_cols) {
    KDTree current = nodes->back().first;
    int64_t column = (nodes->back().second + 1) % n_of_cols;
    nodes->pop_back();

    if (current->left_rows.size() > THRESHOLD)
    {
        current->left = CreateNode(column, find_median(current->left_rows, column), current->left_rows);
        current->left_rows = std::vector<Row>();

        nodes->push_back(std::make_pair(current->left, column));
    }

    if (current->right_rows.size() > THRESHOLD)
    {
        current->right = CreateNode(column, find_median(current->right_rows, column), current->right_rows);
        current->right_rows = std::vector<Row>();

        nodes->push_back(std::make_pair(current->right, column));
    }

}

void freeKDTree(KDTree tree){
    if(tree == NULL) return;

    if(tree->left != NULL){
        freeKDTree(tree->left);
    }

    if(tree->right != NULL){
        freeKDTree(tree->right);
    }
    delete tree;
}