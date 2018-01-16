#include "kd_tree.h"

int64_t THRESHOLD = 100;

struct KDNode{
    ElementType element;
    int64_t column;

    KDNode* left; // less than element
    KDNode* right; // greater or equal than element

    std::vector<Row> left_rows;
    std::vector<Row> right_rows;
};

KDTree CreateNode(int64_t column, ElementType element, std::vector<Row>&lines){
    KDTree node = (KDTree) malloc(sizeof(KDNode));
    node->column = column;
    node->element = element;
    node->left = NULL;
    node->right = NULL;
    for (size_t i = 0; i < lines.size(); ++i) {
        if(lines.at(i).data.at(column) < element)
            node->left_rows.push_back(lines.at(i));
        else
            node->right_rows.push_back(lines.at(i));
    }
    return node;
}

KDTree CheckLeftSide(KDTree current, int64_t column, ElementType element){
    if(current->left == NULL){
        if(current->left_rows.size() > THRESHOLD) {
            current->left = CreateNode(column, element, current->left_rows);
            current->left_rows = std::vector<Row>();
        }
        return NULL;
    }else{
        return current->left;
    }
}

KDTree CheckRightSide(KDTree current, int64_t column, ElementType element){
    if(current->right == NULL) {
        if (current->right_rows.size() > THRESHOLD){
            current->right = CreateNode(column, element, current->right_rows);
            current->right_rows = std::vector<Row>();
        }
        return NULL;
    }else{
        return current->right;
    }
}

KDTree Insert(KDTree tree, int64_t column, ElementType element, std::vector<Row>&lines){
    if(tree == NULL){
        return CreateNode(column, element, lines);
    }

    std::vector<KDTree> nodes_to_check;
    nodes_to_check.push_back(tree);
    while(!nodes_to_check.empty()){
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back(); // delete the last element

        if(current->column == column){
            if(current->element == element)
                continue;
            else if(current->element < element){
                KDTree next_node = CheckLeftSide(current, column, element);
                if(next_node != NULL)
                    nodes_to_check.push_back(next_node);
            }else{
                KDTree next_node = CheckRightSide(current, column, element);
                if(next_node != NULL)
                    nodes_to_check.push_back(next_node);
            }
        }else{
            KDTree next_node = CheckLeftSide(current, column, element);
            if(next_node != NULL)
                nodes_to_check.push_back(next_node);

            next_node = CheckRightSide(current, column, element);
            if(next_node != NULL)
                nodes_to_check.push_back(next_node);
        }
    }
    return tree;
}

std::vector<int64_t > SearchKDTree(KDTree index, std::vector<std::pair<int64_t, int64_t>> query, std::vector<Row> lines){
    std::vector<int64_t> ids;

    for (size_t col = 0; col < query.size(); ++col) {
        int64_t lowKey = query.at(col).first;
        int64_t highKey = query.at(col).second;

        index = Insert(index, col,lowKey, lines);
        index = Insert(index, col,highKey, lines);
    }

    std::vector<KDTree> nodes_to_check;
    nodes_to_check.push_back(index);
    while(!nodes_to_check.empty()) {
        KDTree current = nodes_to_check.back();
        nodes_to_check.pop_back(); // delete the last element

        if(query.at(current->column).second <= current->element){
            if(current->left == NULL){
                for (size_t i = 0; i < current->left_rows.size(); ++i)
                    ids.push_back(current->left_rows.at(i).id);
            }else{
                nodes_to_check.push_back(current->left);
            }

        }else if(query.at(current->column).first >= current->element){
            if(current->right == NULL){
                for (size_t i = 0; i < current->right_rows.size(); ++i)
                    ids.push_back(current->right_rows.at(i).id);
            }else{
                nodes_to_check.push_back(current->right);
            }
        }else{
            if(current->left == NULL){
                for (size_t i = 0; i < current->left_rows.size(); ++i)
                    ids.push_back(current->left_rows.at(i).id);
            }else{
                nodes_to_check.push_back(current->left);
            }

            if(current->right == NULL){
                for (size_t i = 0; i < current->right_rows.size(); ++i)
                    ids.push_back(current->right_rows.at(i).id);
            }else{
                nodes_to_check.push_back(current->right);
            }
        }
    }

    return ids;
}