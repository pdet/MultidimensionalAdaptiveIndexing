//
// Created by PHolanda on 17/12/17.
// Adapted from Saarland University Uncracked Pieces Simulator
//

#ifndef LIGHTWEIGHTADAPTIVEINDEXING_BULKLOADING_BP_TREE_H
#define LIGHTWEIGHTADAPTIVEINDEXING_BULKLOADING_BP_TREE_H

#include <vector>
#include "../cracking/cracker_index.h"

typedef int colKey_t;
typedef int rowId_t;

typedef enum _NodeType {
    InnerNodeType,
    LeafNodeType
} NodeType;

class InnerNode;
class LeafNode;

class BPNode {

protected:

    const NodeType m_nodeType;

    int m_currentNumberOfEntries;
    colKey_t* m_keys;

    int m_currentNumberOfNodes;
    BPNode** m_pointers;

    BPNode(NodeType nodeType)
            : m_nodeType(nodeType)
            , m_currentNumberOfEntries(0)
            , m_keys(NULL)
            , m_currentNumberOfNodes(0)
            , m_pointers(NULL)
            , m_fatherNode(NULL)
    {}

    ~BPNode() {
        if(m_keys) {
            delete[] m_keys;
            m_keys = NULL;
        }
        if(m_pointers) {

        }
    }

public:

    static int m_maxNumberOfEntries;

    BPNode* m_fatherNode;

    bool isFull();
    void addKey(const colKey_t& key);

    const int& getKey(const int position);
    void removeKey(const int position);

    void addPointer(BPNode* const node);
    BPNode* const getPointer(const int position);
    void removePointer(const int position);

    const int numberOfKeys();

    BPNode* split(BPNode*& root);
    const LeafNode* lookup(const colKey_t& key);

    const NodeType& getNodeType() const {
        return m_nodeType;
    }
};

class InnerNode : public BPNode {

public:

    InnerNode() : BPNode(InnerNodeType) {}
};

class LeafNode : public BPNode {

private:

    LeafNode* m_previous;
    LeafNode* m_next;
    bool m_isOverflowNode;
    IndexEntry* m_currentOffset;

public:

    LeafNode(IndexEntry* currentOffset)
            : BPNode(LeafNodeType)
            , m_previous(NULL)
            , m_next(NULL)
            , m_isOverflowNode(false)
            , m_currentOffset(currentOffset)
    {}

    LeafNode(IndexEntry* currentOffset, bool overflowNode)
            : BPNode(LeafNodeType)
            , m_previous(NULL)
            , m_next(NULL)
            , m_isOverflowNode(overflowNode)
            , m_currentOffset(currentOffset)
    {}

    void addRowId(const rowId_t& rowId);
    void addKey(const colKey_t& key);
    rowId_t getRowId(const colKey_t& key) const;
    rowId_t getGTE(const colKey_t& key) const;
    rowId_t getLT(const colKey_t& key) const;
    void setNext(LeafNode* next);
    void setPrevious(LeafNode* previous);
    LeafNode* getPrevious();
    LeafNode* getNext();
    void setAsOverflowNode() { m_isOverflowNode = true; }

};



class BulkBPTree {

private:

    BPNode* m_root;
    LeafNode* m_currentLeaf;

public:

    typedef std::pair<colKey_t, rowId_t> keyValuePair_t;

public:

    BulkBPTree(IndexEntry* data, int size);

    rowId_t lookup(const colKey_t& key);
    rowId_t gte(const colKey_t& key);
    rowId_t lt(const colKey_t& key);
};

void *build_bptree_bulk(IndexEntry *c, int n);


#endif /* BULKBPTREE_HPP_ */



