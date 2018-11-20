#include <vector>

class DSU {
private:
    std::vector<int> p;
public:
    DSU(int size) {
        p.resize(size);
        for(int i = 0; i < size; ++i) {
            p[i] = i;
        }
    }

    int find(int v) {
        if(p[v] == v) {
            return v;
        }
        return p[v] = find(p[v]);
    }

    void merge(int a, int b) {
        int _a = find(a);
        int _b = find(b);
        if(_a != _b) {
            p[_b] = _a;
        }
    }
};