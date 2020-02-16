#pragma once

#include <vector>

class DSU {
private:
    std::vector<int> p;
public:
    explicit DSU(const int & size) {
        p.resize(size);
        for(int i = 0; i < size; ++i) {
            p[i] = i;
        }
    }

    int find(const int & v) {
        if(p[v] == v) {
            return v;
        }
        return p[v] = find(p[v]);
    }

    void merge(const int & a, const int & b) {
        int _a = find(a);
        int _b = find(b);
        if(_a != _b) {
            p[_b] = _a;
        }
    }
};