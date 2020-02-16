#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <assert.h>
#include <map>
#include <queue>
#include <set>

using namespace std;


void solve()
{
    // n - number of vertexes
    // k - number of edges
    int n, k;
    cin >> n >> k;

    // f - current flow via edge
    // c - edge capacity
    using edge = struct {
        int64_t f;
        int64_t c;
        bool is_back = false;
    };
    vector<edge> edges;

    // to - vertex id
    // e_id - edge id from edges array
    using node = struct {
        int to;
        size_t e_id;
    };
    vector<vector<node> > g(n);

    size_t start_id(0), end_id(n-1);

    // Read input data
    int u, v;
    int64_t c;
    for (int i = 0; i < k; ++i)
    {
        cin >> u >> v >> c;
        --u, --v;

        // Add new edge with C capacity and 0 flow
        edges.push_back({ 0, c });
        g[u].push_back({ v, edges.size() - 1 });

        // Due to graph edges are not directed we add back edge 
        // with C capacity. In case of directed edges capacity should be 0.
        edges.push_back({ 0, c , true});
        g[v].push_back({ u, edges.size() - 1 });
    }

    auto find_flow = [&]() -> bool {
        int64_t max_flow(0);
        queue<pair<int, int64_t> > q;
        vector<bool> used(g.size(), false);
        vector<pair<int, int64_t> > parent(g.size(), { -1, -1 });
        q.push({ 0, 1e9 });
        used[0] = true;
        while (!q.empty())
        {
            int from = q.front().first;
            int64_t flow = q.front().second;
            q.pop();

            if (from == end_id)
            {
                max_flow = flow;
                break;
            }

            for (auto & node : g[from])
            {
                if (!used[node.to] && edges[node.e_id].c - edges[node.e_id].f > 0)
                {
                    q.push({ node.to, min(flow,  edges[node.e_id].c - edges[node.e_id].f) });
                    parent[node.to] = { from, node.e_id };
                    used[node.to] = true;
                }
            }
        }

        if (max_flow > 0)
        {
            int to = end_id;
            while (parent[to].first != -1)
            {
                int from = parent[to].first;
                int e_id = parent[to].second;
                
                edges[e_id].f += max_flow;
                edges[e_id ^ 1].f -= max_flow;
                to = from;
            }
            return true;
        }

        return false;
    };

    while (find_flow());

    int64_t max_flow = 0;
    for (auto & to : g[0])
    {
        max_flow += edges[to.e_id].f;
    }

    cout << max_flow << endl;
    for (auto & e : edges)
    {
        if (!e.is_back)
            cout << e.f << endl;
    }
}
