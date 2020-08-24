#pragma once
#include <cstring>
#include <memory>

class CandidateList {
public:
    CandidateList(CandidateList &list) {
        this->size = list.size;
        this->data = move(list.data);
        this->capacity = list.capacity;
    }

    size_t size;
    size_t capacity;
    std::unique_ptr<uint32_t []>data;

    explicit CandidateList(size_t total_capacity = 0) {

        if (total_capacity == 0) {
            this->size = 0;
            this->capacity = 1024;
        } else {
            this->size = 0;
            this->capacity = total_capacity;
        }
        data = std::make_unique<uint32_t[]>(this->capacity);
    };

//    ~CandidateList(){
////        size = 0;
////        capacity = 0;
////        if (data){
////            data.release();
////        }
//
//    };

    void push_back(uint32_t value) {
        if (size == capacity) {
            auto new_data = std::unique_ptr<uint32_t []>(new uint32_t [capacity*2]);
            std::memcpy(new_data.get(), data.get(), capacity * sizeof(uint32_t));
            capacity *= 2;
            data = move(new_data);
        }
        data[size] = value;
        size++;
    }

    void maybe_push_back(uint32_t value,int match) {
        if (size == capacity) {
            auto new_data = std::unique_ptr<uint32_t []>(new uint32_t [capacity*2]);
            std::memcpy(new_data.get(), data.get(), capacity * sizeof(uint32_t));
            capacity *= 2;
            data = move(new_data);
        }
        data[size] = value;
        size+=match;
    }

    void initialize(CandidateList list) {
        this->size = list.size;
        this->data = move(list.data);
        this->capacity = list.capacity;
    }

    uint32_t get(size_t offset) const {
        return  data[offset];
    }
};
