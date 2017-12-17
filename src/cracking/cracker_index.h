//
// Created by PHolanda on 17/12/17.
//

//
// Adapted from Saarland University Uncracked Pieces Simulator
//

#include <vector>
#include <cstdlib>
#include <iostream>

#include <iomanip>

#include <cmath>

#ifndef LIGHTWEIGHTADAPTIVEINDEXING_CRACKERINDEX_H
#define LIGHTWEIGHTADAPTIVEINDEXING_CRACKERINDEX_H

class IndexEntry {
public:


    int64_t m_key;
    int64_t m_rowId;

    IndexEntry(int64_t i)
            : m_key(i)
            , m_rowId(-1)
    {
    }

    IndexEntry()
            : m_key(-1)
            , m_rowId(-1)
    {}

    IndexEntry(int64_t key, int64_t rowId)
            : m_key(key)
            , m_rowId(rowId)
    {}
//Query comparisons
    bool operator>(int64_t& other) const { return m_key > other; }
    bool operator>=(int64_t& other) const { return m_key >= other; }
    bool operator<(int64_t& other) const { return m_key < other; }
    bool operator<=(int64_t& other) const { return m_key <= other; }
    bool operator!=(int64_t& other) const { return m_key != other; }
    bool operator==(int64_t& other) const { return m_key == other; }

    bool operator>(const IndexEntry& other) const { return m_key > other.m_key; }
    bool operator>=(const IndexEntry& other) const { return m_key >= other.m_key; }
    bool operator<(const IndexEntry& other) const { return m_key < other.m_key; }
    bool operator<=(const IndexEntry& other) const { return m_key <= other.m_key; }
    bool operator!=(const IndexEntry& other) const { return m_key != other.m_key; }

};

#endif //LIGHTWEIGHTADAPTIVEINDEXING_CRACKERINDEX_H
