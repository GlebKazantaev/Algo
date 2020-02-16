#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <queue>

using namespace std;

class Vertex
{
    Vertex(shared_ptr<Vertex> pred, char ch) : pred(pred), ch(ch) {}

    Vertex() : Vertex(nullptr, 0) {}
public:
    using ptr = std::shared_ptr<Vertex>;

    Vertex::ptr next[26], go_link[26];

    Vertex::ptr link = nullptr;
    Vertex::ptr super_link = nullptr;
    Vertex::ptr pred = nullptr;
    
    char ch;

    bool used = false;
    bool is_root = false;
    bool is_finish = false;
    vector<int> finish_words_id;

    int counter = 0;
    int depth = 0;

    static Vertex::ptr create(shared_ptr<Vertex> pred, char ch)
    {
        assert(pred);
        Vertex * v = new Vertex(pred, ch);
        v->depth = pred->depth + 1;
        return std::shared_ptr<Vertex>(v);
    }

    static Vertex::ptr create_root()
    {
        Vertex * v = new Vertex();
        auto ptr = std::shared_ptr<Vertex>(v);
        ptr->link = ptr;
        ptr->is_root = true;
        return ptr;
    }   

    Vertex::ptr go(char ch)
    {
        int id = ch - 'a';
        if (!go_link[id])
        {
            if (next[id])
            {
                go_link[id] = next[id];
            }
            else
            {
                if (is_root)
                {
                    go_link[id] = link;
                }
                else
                {
                    go_link[id] = get_link()->go(ch);
                }
            }
        }
        return go_link[id];
    }

    Vertex::ptr get_link()
    {
        if (!link)
        { 
            if (pred->is_root)
            {
                link = pred;
            }
            else
            {
                link = pred->get_link()->go(ch);
            }
        }
        return link;
    }

    Vertex::ptr get_super_link()
    {
        if (!super_link)
        {
            auto suff = get_link();
            if (suff->is_finish)
            {
                super_link = suff;
            }
            else
            {
                super_link = suff->get_super_link();
            }
        }
        return super_link;
    }
};

void add_word(Vertex::ptr root, const string & str)
{
    static int new_word_id = 0;

    Vertex::ptr v(root);

    int pos = 0;
    for (auto & ch : str)
    {
        int id = ch - 'a';
        if (auto & next = v->next[id])
        {
            v = next;
        }
        else
        {
            v->next[id] = Vertex::create(v, ch);
            v = v->next[id];
        }
    }

    v->is_finish = true;
    v->finish_words_id.push_back(new_word_id++);
}

void bfs(Vertex::ptr root, vector<Vertex::ptr> & order)
{
    queue<Vertex::ptr> q;
    q.push(root);
    while (!q.empty())
    {
        auto v = q.front();
        q.pop();

        order.push_back(v);

        for (auto & next : v->next)
        {
            if (next) q.push(next);
        }
    }
}

void solve()
{
    Vertex::ptr root = Vertex::create_root();

    int n;
    cin >> n;
    for (int i = 0; i < n; ++i)
    {
        string str;
        cin >> str;
        add_word(root, str);
    }
    
    vector<int> ans(n, 0);

    auto current_vertex = root;

    string text;
    cin >> text;
    for (auto & ch : text)
    {
        current_vertex = current_vertex->go(ch);
        current_vertex->counter++;
        current_vertex->used = true;
    }

    vector<Vertex::ptr> order;
    bfs(root, order);

    for (int i = order.size() - 1; i >= 0; --i)
    {
        auto v = order[i];
        if (v->counter > 0)
        {
            if (v->is_finish)
            {
                for (auto wor_id : v->finish_words_id)
                    ans[wor_id] = v->counter;
            }
            auto link = v->get_link();
            link->counter += v->counter;
        }
    }

    for (auto & count : ans)
    {
        printf("%d\n", count);
    }
}