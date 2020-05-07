#pragma once

class CandidateList {
public:
    CandidateList(CandidateList &list) {
        this->size = list.size;
        this->data = move(list.data);
        this->capacity = list.capacity;
    }

    size_t size;
    size_t capacity;
    unique_ptr<uint32_t *>data;

    CandidateList(size_t total_capacity = 0) {
        data = make_unique<uint32_t*>();
        if (total_capacity == 0) {
            this->size = 0;
            this->capacity = 1024;
        } else {
            this->size = 0;
            this->capacity = total_capacity;
        }
        *data = (uint32_t *) malloc(sizeof(uint32_t) * this->capacity);
    };

    ~CandidateList(){};

    void push_back(uint32_t value) {
        if (size == capacity) {
            this->capacity *= 2;
            *data = (uint32_t *) realloc(*data, sizeof(uint32_t) * this->capacity);
        }
        (*data)[size] = value;
        size++;
    }

    void maybe_push_back(uint32_t value,int match) {
        if (size == capacity) {
            this->capacity *= 2;
            *data = (uint32_t *) realloc(*data, sizeof(uint32_t) * this->capacity);
        }
        (*data)[size] = value;
        size+=match;
    }

    void initialize(CandidateList list) {
        this->size = list.size;
        this->data = move(list.data);
        this->capacity = list.capacity;
    }

    uint32_t get(size_t offset) {
        return  (*data)[offset];
    }
};
