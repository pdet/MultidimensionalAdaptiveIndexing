#ifndef CRACKING_KD_TREE_MINE_HPP
#define CRACKING_KD_TREE_MINE_HPP

#include "kd_tree/kd_tree.hpp"
#include "full_scan.hpp"
#include "cracking_kd_tree.hpp"
#include <string>
#include <map>
#include <set>

#include <cstdlib>

template< typename T>
class MyStack{
private:
    T* stack;
    size_t size = 0;
    size_t capacity = 8;
public:
    MyStack(){
        stack = static_cast<T*>(malloc(capacity * sizeof(T)));
    }

    ~MyStack(){
        free(stack);
    }

    void push_back(const T v){
        if(size == capacity){
            capacity = capacity * 2;
            stack = static_cast<T*>(realloc(stack, capacity * sizeof(T)));
        }
        stack[size] = v;
        size++;
    }

    T pop_back(){
        size--;
        auto v = stack[size];
        return v;
    }

    bool empty(){
        return size == 0;
    }
};

class CrackingKDTreeMine : public CrackingKDTree 
{
    public:
    static const size_t ID = 100;
    CrackingKDTreeMine(std::map<std::string, std::string> config);
    ~CrackingKDTreeMine();

    string name() override{
        return "CrackingKDTreeMine";
    }

private:
    void insert_edge(Edge& edge) override;
};

#endif
