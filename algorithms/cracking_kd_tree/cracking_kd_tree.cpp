#include "cracking_kd_tree.h"

void CrackingKdTree::pre_processing(
    vector<int64_t> &ids,
    vector<vector<int64_t> > &columns
){
    number_of_columns = columns.size();
    data_size = ids.size();
    index = nullptr;
    threshold = 2000;
    this->ids.assign(ids.begin(), ids.end());
    this->columns.resize(number_of_columns);

    for(size_t i = 0; i < number_of_columns; i++)
    {
        this->columns.at(i).reserve(data_size);
        for(size_t j = 0; j < columns.at(i).size(); j++)
        {
            this->columns.at(i)[j] = columns.at(i).at(j);
        }
    }
}

void CrackingKdTree::partial_index_build(
    vector<array<int64_t, 3> > &query
){
    for (size_t query_num = 0; query_num < query.size(); query_num++){
        int64_t low = query.at(query_num).at(0);
        int64_t high = query.at(query_num).at(1);
        int64_t col = query.at(query_num).at(2);
        if(low != -1)
            insert(index, col, low);
        if(high != -1)
            insert(index, col, high);
    }
}

void CrackingKdTree::search(
    vector<array<int64_t, 3> > &query
){
    offsets.resize(0);
    vector<Tree> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    lower_limits.push_back(0);
    upper_limits.push_back(data_size - 1);
    nodes_to_check.push_back(index);
    while (!nodes_to_check.empty())
    {
        Tree current = nodes_to_check.back();
        nodes_to_check.pop_back();

        int64_t lower_limit = lower_limits.back();
        lower_limits.pop_back();

        int64_t upper_limit = upper_limits.back();
        upper_limits.pop_back();

        // Check if current column is not in the query
        if(!node_in_query(current, query)){
            if (current->left_position != -1)
            {
                if (current->Left == NULL)
                {
                    offsets.push_back(make_pair(lower_limit, current->left_position));
                }
                else
                {
                    nodes_to_check.push_back(current->Left);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                if (current->Right == NULL)
                {
                    offsets.push_back(make_pair(current->right_position, upper_limit));
                }
                else
                {
                    nodes_to_check.push_back(current->Right);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
        else if (node_greater_equal_query(current, query))
        {
            if (current->left_position != -1)
            {
                if (current->Left == NULL)
                {
                    offsets.push_back(make_pair(lower_limit, current->left_position));
                }
                else
                {
                    nodes_to_check.push_back(current->Left);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
        }
        else if (node_less_equal_query(current, query))
        {
            if (current->right_position != -1)
            {
                if (current->Right == NULL)
                {
                    offsets.push_back(make_pair(current->right_position, upper_limit));
                }
                else
                {
                    nodes_to_check.push_back(current->Right);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
        else
        {
            if (current->left_position != -1)
            {
                if (current->Left == NULL)
                {
                    offsets.push_back(make_pair(lower_limit, current->left_position));
                }
                else
                {
                    nodes_to_check.push_back(current->Left);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                if (current->Right == NULL)
                {
                    offsets.push_back(make_pair(current->right_position, upper_limit));
                }
                else
                {
                    nodes_to_check.push_back(current->Right);
                    lower_limits.push_back(current->right_position);
                    upper_limits.push_back(upper_limit);
                }
            }
        }
    }
}

void CrackingKdTree::scan(
    vector<array<int64_t, 3> > &query
){
    resulting_ids.resize(0);
    for(size_t i = 0; i < offsets.size(); ++i){
        int64_t sel_size;
        int64_t *sel_vector =(int64_t*) malloc(sizeof(int64_t) * offsets.at(i).second - offsets.at(i).first + 1);
        int64_t low = query.at(0).at(0);
		int64_t high = query.at(0).at(1);
		int64_t col = query.at(0).at(2);
        sel_size = select_rq_scan_new (sel_vector, &columns[col][offsets.at(i).first],low,high,offsets.at(i).second - offsets.at(i).first + 1);
        for (size_t query_num = 1; query_num < query.size(); query_num++)
        {
            int64_t low = query.at(query_num).at(0);
			int64_t high = query.at(query_num).at(1);
			int64_t col = query.at(query_num).at(2);
            sel_size = select_rq_scan_sel_vec(sel_vector, &columns[col][offsets.at(i).first],low,high,sel_size);
        }
        for (size_t j = 0; j < sel_size; ++j)
            resulting_ids.push_back(ids[sel_vector[j] + offsets.at(i).first]);
    }
}

vector<int64_t> CrackingKdTree::get_result(){
    return resulting_ids;
}


// PRIVATE METHODS

int64_t CrackingKdTree::select_rq_scan_sel_vec(
    int64_t*__restrict__ sel,
    int64_t*__restrict__ col,
    int64_t keyL, int64_t keyH, int64_t n
){
    int64_t j;
    for (int64_t i = j = 0; i < n; i++){
        int matching =  ((keyL <= col[sel[i]]) || (keyL == -1)) &&  ((col[sel[i]] < keyH) || (keyH == -1));
        sel[j] = sel[i];
        j += matching;
    }
    return j;

}

int64_t CrackingKdTree::select_rq_scan_new (
    int64_t*__restrict__ sel,
    int64_t*__restrict__ col,
    int64_t keyL, int64_t keyH, int64_t n
){
    int64_t j;
    for (int64_t i = j = 0 ; i < n; i++){
        int matching =  ((keyL <= col[i]) || (keyL == -1)) &&  ((col[i] < keyH) || (keyH == -1));

        sel[j] = i;
        j += matching;

    }
    return j;
}

void CrackingKdTree::exchange(int64_t x1, int64_t x2)
{
    if (x1 == x2)
        return;
    int64_t tmp;
    tmp = ids.at(x1);
    ids.at(x1) = ids.at(x2);
    ids.at(x2) = tmp;

    for (size_t i = 0; i < number_of_columns; i++)
    {
        tmp = columns.at(i).at(x1);
        columns.at(i).at(x1) = columns.at(i).at(x2);
        columns.at(i).at(x2) = tmp;
    }
}

// Cracks table from position i = low_ until i == high_, on determined column with the element
int64_t CrackingKdTree::crack_table(int64_t low, int64_t high, int64_t element, int64_t c)
{
    int64_t x1 = low;
    int64_t x2 = high;

    while (x1 <= x2)
    {
        if (columns.at(c).at(x1) < element)
            x1++;
        else
        {
            while (x2 >= x1 && (columns.at(c).at(x2) >= element))
                x2--;
            if (x1 < x2)
            {
                exchange(x1, x2);
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

Tree CrackingKdTree::create_node(int64_t column, int64_t element, int64_t left_position, int64_t right_position)
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

Tree CrackingKdTree::check_left_side(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit)
{
    if (current->Left == NULL)
    {
        if (upper_limit - lower_limit + 1 > threshold)
        {
            int64_t position = crack_table(lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->Left = create_node(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->Left = create_node(column, element, upper_limit, -1);
            }
            else
            {
                current->Left = create_node(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->Left;
    }
}

Tree CrackingKdTree::check_right_side(Tree current, int64_t column, int64_t element, int64_t lower_limit, int64_t upper_limit)
{
    if (current->Right == NULL)
    {
        if (upper_limit - lower_limit + 1 > threshold)
        {
            int64_t position = crack_table(lower_limit, upper_limit, element, column);

            if (position < lower_limit)
            {
                current->Right = create_node(column, element, -1, lower_limit);
            }
            else if (position >= upper_limit)
            {
                current->Right = create_node(column, element, upper_limit, -1);
            }
            else
            {
                current->Right = create_node(column, element, position, position + 1);
            }
        }
        return NULL;
    }
    else
    {
        return current->Right;
    }
}

void CrackingKdTree::insert_into_root(Tree &tree, int64_t column, int64_t element)
{
    size_t lower_limit = 0;
    size_t upper_limit = data_size - 1;
    int64_t position = crack_table(lower_limit, upper_limit, element, column);

    if (position < lower_limit)
    {
        tree = create_node(column, element, -1, 0);
    }
    else if (position >= upper_limit)
    {
        tree = create_node(column, element, upper_limit, -1);
    }
    else
    {
        tree = create_node(column, element, position, position + 1);
    }
}

void CrackingKdTree::insert(Tree &tree, int64_t column, int64_t element)
{
    if (!tree)
    {
        return insert_into_root(tree, column, element);
    }

    vector<Tree> nodes_to_check;
    vector<int64_t> lower_limits, upper_limits;

    lower_limits.push_back(0);
    upper_limits.push_back(data_size - 1);
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
                    Tree new_node = check_left_side(current, column, element, lower_limit, upper_limit);
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
                    Tree new_node = check_right_side(current, column, element, lower_limit, upper_limit);
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
                Tree new_node = check_left_side(current, column, element, lower_limit, current->left_position);
                if (new_node != NULL)
                {
                    nodes_to_check.push_back(new_node);
                    lower_limits.push_back(lower_limit);
                    upper_limits.push_back(current->left_position);
                }
            }
            if (current->right_position != -1)
            {
                Tree new_node = check_right_side(current, column, element, current->right_position, upper_limit);
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

bool CrackingKdTree::node_in_query(Tree current, vector<array<int64_t, 3> > &query){
    bool inside = false;
    for(size_t i = 0; i < query.size(); i++)
    {
        if(current->column == query.at(i).at(2))
            inside = true;
    }
    return inside;
}

bool CrackingKdTree::node_greater_equal_query(Tree node, vector<array<int64_t, 3> > &query){
    for(size_t i = 0; i < query.size(); i++)
    {
        if(node->column == query.at(i).at(2)){
            int64_t high = query.at(i).at(1);
            if(high == -1)
                return false;
            else
                return high <= node->Element;
        }
    }
    return false;
}

bool CrackingKdTree::node_less_equal_query(Tree node, vector<array<int64_t, 3> > &query){
    for(size_t i = 0; i < query.size(); i++)
    {
        if(node->column == query.at(i).at(2)){
            int64_t low = query.at(i).at(0);
            if(low == -1)
                return false;
            else
                return node->Element <= low;
        }
    }
    return false;
}