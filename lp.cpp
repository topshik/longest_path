#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <deque>
#include <iostream>
#include <iterator>
#include <limits>
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Graph {
public:
    using Vertex = size_t;
    using VertexSet = std::unordered_set<Vertex>;
    using AdjencyList = std::unordered_map<Vertex, VertexSet>;

    void AddVertex(Vertex v) {
        adjency_list_[v];
    }

    void AddEdge(Vertex u, Vertex v) {
        adjency_list_[u].insert(v);
        adjency_list_[v].insert(u);
    }

    const VertexSet& AdjecentVertices(Vertex v) const {
        const auto it = adjency_list_.find(v);
        if (it != adjency_list_.end()) {
            return it->second;
        } else {
            return empty_set_;
        }
    }

    VertexSet AllVertices() const {
        VertexSet vs;
        vs.reserve(adjency_list_.size());
        for (const auto& pair : adjency_list_) {
            const auto& vertex = pair.first;
            vs.insert(vertex);
        }
        return vs;
    }

    Vertex RandVertex() const {
        return rand() % AllVertices().size();
    }

    const AdjencyList& AsAdjencyList() const {
        return adjency_list_;
    }

private:
    AdjencyList adjency_list_;
    static const VertexSet empty_set_;
};

const Graph::VertexSet Graph::empty_set_;

class Path {
private:
    std::deque<Graph::Vertex> path_;
    Graph graph_;

public:
    explicit Path(const Graph& graph)
    : graph_(graph), path_(std::deque<Graph::Vertex>(1, rand() % graph.AllVertices().size() + 1)) {}

    void Add(Graph::Vertex v) {
        Graph::VertexSet front_adjescent = graph_.AdjecentVertices(path_.front());
        Graph::VertexSet back_adjescent = graph_.AdjecentVertices(path_.back());
        bool is_front_adjescent = front_adjescent.find(v) != front_adjescent.end();
        bool is_back_adjescent = back_adjescent.find(v) != back_adjescent.end();
        if (is_front_adjescent && is_back_adjescent)
            if (rand() % 2)
                path_.push_front(v);
            else
                path_.push_back(v);
            else if (is_front_adjescent)
                path_.push_front(v);
            else if (is_back_adjescent)
                path_.push_back(v);
    }

    void Remove(Graph::Vertex v) {
        if (path_.front() == v)
            path_.pop_front();
        else if (path_.back() == v)
            path_.pop_back();
    }

    size_t Cost() const {
        return path_.size();
    }

    Graph::Vertex front() {
        return path_.front();
    }

    Graph::Vertex back() {
        return path_.back();
    }

    std::deque<Graph::Vertex>::const_iterator begin() const {
        return path_.begin();
    }

    std::deque<Graph::Vertex>::const_iterator end() const {
        return path_.end();
    }

    std::deque<Graph::Vertex>::const_iterator find(Graph::Vertex v) {
        return std::find(begin(), end(), v);
    }
};

struct DebugInfo {
    std::vector<size_t> costs;
};

std::ostream& operator<<(std::ostream& out, const DebugInfo& debug_info) {
    for (size_t i = 0; i < debug_info.costs.size(); ++i) {
        out << i << " " << debug_info.costs[i] << "\n";
    }
    return out;
}

class Metropolis {
private:
    size_t max_iterations = 1000;
public:
    Path Solve(const Graph& graph, double K, double T, bool fire, DebugInfo &debug_info) const {
        Path path = Path(graph);
        for (size_t i = 0; i != max_iterations; ++i) {
            if (path.Cost() == 0) {
                path = Path(graph);
            }
            Graph::VertexSet cands;
            cands.insert(path.front());
            cands.insert(path.back());
            for (auto v : graph.AdjecentVertices(path.front())) {
                if (path.find(v) == path.end()) {
                    cands.insert(v);
                }

            }
            for (auto v : graph.AdjecentVertices(path.back())) {
                if (path.find(v) == path.end())
                    cands.insert(v);
            }
            auto tempit = cands.begin();
            std::advance(tempit, rand()%cands.size());
            auto v = *tempit;
            if (v != path.front() && v != path.back()) {
                path.Add(v);
            } else {
                if (rand() / RAND_MAX < exp(- (1 / (K * T))))
                    path.Remove(v);
            }
            if (fire) {
                T = sqrt(T);
            }
            debug_info.costs.push_back(path.Cost());
        }
        return path;
    }
};

class GradientDescent {
private:
    size_t max_iterations = 1000;
public:
    Path Solve(const Graph& graph, DebugInfo &debug_info) const {
        Path path = Path(graph);
        for (size_t i = 0; i != max_iterations; ++i) {
            if (path.Cost() == 0) {
                path = Path(graph);
            }
            Graph::VertexSet cands;
            for (auto v : graph.AdjecentVertices(path.front())) {
                if (path.find(v) == path.end()) {
                    cands.insert(v);
                }

            }
            for (auto v : graph.AdjecentVertices(path.back())) {
                if (path.find(v) == path.end())
                    cands.insert(v);
            }
            if (cands.size() == 0)
                break;
            auto tempit = cands.begin();
            std::advance(tempit, rand()%cands.size());
            auto v = *tempit;
            path.Add(v);
            debug_info.costs.push_back(path.Cost());
        }
        return path;
    }
};

Graph RandomGraph(size_t size, double edge_probability) {
    Graph graph;
    for (Graph::Vertex v = 1; v <= size; ++v) {
        graph.AddVertex(v);
    }
    for (Graph::Vertex v = 1; v <= size; ++v) {
        for (Graph::Vertex u = v + 1; u <= size; ++u) {
            if (double(rand()) / RAND_MAX <= edge_probability) {
                graph.AddEdge(v, u);
            }
        }
    }
    return graph;
}

Graph StarGraph(size_t size) {
    Graph graph;
    for (Graph::Vertex v = 2; v <= size; ++v) {
        graph.AddEdge(1, v);
    }
    return graph;
}

int InitRandSeed(int argc, const char* argv[]) {
    int rand_seed;
    if (argc >= 2) {
        rand_seed = atoi(argv[1]);
    } else {
        rand_seed = time(nullptr);
    }
    srand(rand_seed);
    return rand_seed;
}

int main(int argc, const char* argv[]) {
    std::cout << "Using rand seed: " << InitRandSeed(argc, argv) << "\n";
    Metropolis metropolis;
    GradientDescent descent;
    DebugInfo debug_info_metropolis1, debug_info_metropolis2, debug_info_descent;
    for (size_t i = 0; i != 1; ++i) {
        const auto graph = RandomGraph(100, 0.1);
        double average1 = 0, average2 = 0, average3 = 0;
        for (size_t j = 0; j != 100; ++j) {
            debug_info_metropolis1.costs.clear();
            debug_info_metropolis2.costs.clear();
            debug_info_descent.costs.clear();
            Path path_metropolis1(graph), path_metropolis2(graph), path_descent(graph);
            path_metropolis1 = metropolis.Solve(graph, 1, 1000, true, debug_info_metropolis1);
            path_metropolis2 = metropolis.Solve(graph, 1, 1000, false, debug_info_metropolis2);
            path_descent = descent.Solve(graph, debug_info_descent);
            average1 += path_metropolis1.Cost();
            average2 += path_metropolis2.Cost();
            average3 += path_descent.Cost();
        }
        average1 /= 100;
        average2 /= 100;
        average3 /= 100;
        std::cout << i << " " << average1 << " "  << average2 << " " << average3 << '\n';
    }

    std::cout << "--------------------\n";
    std::cout << debug_info_metropolis1 << '\n';
    std::cout << "--------------------\n";
    std::cout << debug_info_metropolis2 << '\n';
    std::cout << "--------------------\n";
    std::cout << debug_info_descent << '\n';

    return 0;
}
