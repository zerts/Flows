#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>
#include <climits>
#include <string>
#include <map>

#define in cin
#define out cout

using namespace std;

//ifstream in("input.txt");
//ofstream out("output.txt");

size_t n, m;

vector < pair < size_t, size_t> > input;

class Edge
{
public:
    size_t to, reverse, opposite;
    long long diametr;
    Edge(size_t t, long long d, size_t r, size_t o)
    {
        to = t;
        diametr = d;
        reverse = r;
        opposite = o;
    }
};

class Graph
{
protected:
    size_t vertexNumber;
    vector < pair < vector < Edge >, vector <Edge> > > edge;
public:
    enum EDGE_DIRECTION
    {
        RIGHT,
        REVERSE
    };
    Graph(){};
    Graph(size_t n)
    {
        vertexNumber = n;
        edge.resize(n);
    }
    void addEdge(size_t start, size_t finish, long long diametr)
    {
        if (start < vertexNumber && finish < vertexNumber)
        {
            edge[start].first.push_back(Edge(finish, diametr, edge[finish].first.size(), edge[finish].second.size()));
            edge[finish].first.push_back(Edge(start, 0, edge[start].first.size() - 1, edge[start].second.size()));
            edge[finish].second.push_back(Edge(start, diametr, edge[start].second.size(), edge[start].first.size() - 1));
            edge[start].second.push_back(Edge(finish, 0, edge[finish].second.size() - 1, edge[finish].first.size() - 1));
        }
    }
    size_t getVertexDeg(size_t curr, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return edge[curr].first.size();
        }
        else
        {
            return edge[curr].second.size();
        }
    }
    size_t getEdgeOnPosition(size_t curr, size_t position, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return edge[curr].first[position].to;
        }
        else
        {
            return edge[curr].second[position].to;
        }
    }
    long long getDiametrOfEdgeOnPosition(size_t curr, size_t position, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return edge[curr].first[position].diametr;
        }
        else
        {
            return edge[curr].second[position].diametr;
        }
    }
    size_t getNumberOfVertex()
    {
        return vertexNumber;
    }
    size_t getInf()
    {
        return getNumberOfVertex() * 2;
    }
    long long getInfDiametr()
    {
        return LLONG_MAX;
    }
};


class StreamGraph : public Graph
{
private:
    size_t start, finish;
    long long streamSize;
    vector < pair < vector < long long >, vector <long long> > > edgeWithStream;
public:
    StreamGraph(){};
    StreamGraph(size_t n)
    {
        vertexNumber = n;
        edge.resize(n);
        start = 0;
        finish = n - 1;
        streamSize = 0;
    }
    StreamGraph(size_t n, size_t s, size_t f)
    {
        vertexNumber = n;
        edge.resize(n);
        start = s;
        finish = f;
        streamSize = 0;
    }
    void createStreamNet()
    {
        edgeWithStream.resize(vertexNumber);
        for (size_t i = 0; i < vertexNumber; i++)
        {
            edgeWithStream[i].first.assign(edge[i].first.size(), 0);
            edgeWithStream[i].second.assign(edge[i].second.size(), 0);
        }
    }
    size_t getStartNumber()
    {
        return start;
    }
    size_t getFinishNumber()
    {
        return finish;
    }
    void setStart(size_t newStart)
    {
        start = newStart;
    }
    void setFinish(size_t newFinish)
    {
        finish = newFinish;
    }
    long long getEdgeWithStream(size_t curr, size_t position, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return edgeWithStream[curr].first[position];
        }
        else
        {
            return edgeWithStream[curr].second[position];
        }
    }
    long long getEdgeWithoutStream(size_t curr, size_t position, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return edge[curr].first[position].diametr - edgeWithStream[curr].first[position];
        }
        else
        {
            return edge[curr].second[position].diametr - edgeWithStream[curr].second[position];
        }
    }
    pair <size_t, size_t> getReverseEdge(size_t curr, size_t position, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return make_pair(getEdgeOnPosition(curr, position, EDGE_DIRECTION::RIGHT), edge[curr].first[position].reverse);
        }
        else
        {
            return make_pair(getEdgeOnPosition(curr, position, EDGE_DIRECTION::REVERSE), edge[curr].second[position].reverse);
        }
    }
    void addStreamToEdge(size_t curr, size_t position, long long adding, EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            size_t from = curr, to = edge[curr].first[position].to;
            edgeWithStream[from].first[position] += adding;
            edgeWithStream[to].first[edge[from].first[position].reverse] -= adding;
            edgeWithStream[to].second[edge[from].first[position].opposite] += adding;
            edgeWithStream[from].second[edge[to].first[edge[from].first[position].opposite].reverse] -= adding;
        }
        else
        {
            size_t from = curr, to = edge[curr].second[position].to;
            edgeWithStream[from].second[position] += adding;
            edgeWithStream[to].second[edge[curr].second[position].reverse] -= adding;
            edgeWithStream[to].first[edge[from].second[position].opposite] += adding;
            edgeWithStream[from].first[edge[to].second[edge[from].second[position].opposite].reverse] -= adding;
        }
        
    }
    void setStreamSize()
    {
        streamSize = 0;
        for (size_t i = 0; i < edge[finish].second.size(); i++)
        {
            streamSize += edgeWithStream[finish].second[i];
        }
    }
    void setStreamSize(long long newStreamSize)
    {
        streamSize = newStreamSize;
    }
    long long getStreamSize()
    {
        return streamSize;
    }
    void printStreamNet()
    {
        out << endl;
        for (size_t i = 0; i < vertexNumber; i++)
        {
            for (size_t j = 0; j < edge[i].first.size(); j++)
            {
                if (edgeWithStream[i].first[j] > 0)
                {
                    out << i << ' ' << getEdgeOnPosition(i, j, EDGE_DIRECTION::RIGHT) << ' ' << edgeWithStream[i].first[j] << endl;
                }
            }
        }
    }
    size_t getEndNumber(EDGE_DIRECTION direction)
    {
        if (direction == EDGE_DIRECTION::RIGHT)
        {
            return getFinishNumber();
        }
        else
        {
            return getStartNumber();
        }
    }
};

class StreamFinder
{
private:
    vector <size_t> level;
    vector <long long> potentials;
    void createLevelVector(StreamGraph &graph)
    {
        queue <size_t> que;
        que.push(graph.getStartNumber());
        level.assign(graph.getNumberOfVertex(), graph.getInf());
        level[graph.getStartNumber()] = 0;
        while (!que.empty())
        {
            size_t curr = que.front();
            for (size_t i = 0; i < graph.getVertexDeg(curr, Graph::EDGE_DIRECTION::RIGHT); i++)
            {
                size_t next = graph.getEdgeOnPosition(curr, i, Graph::EDGE_DIRECTION::RIGHT);
                if (graph.getEdgeWithoutStream(curr, i, Graph::EDGE_DIRECTION::RIGHT) > 0 && level[next] == graph.getInf())
                {
                    que.push(next);
                    level[next] = level[curr] + 1;
                }
            }
            que.pop();
        }
    }
    size_t makeCurrLevel(size_t currLevel, Graph::EDGE_DIRECTION mode)
    {
        if (mode == Graph::EDGE_DIRECTION::RIGHT)
        {
            return currLevel + 1;
        }
        else
        {
            return currLevel - 1;
        }
    }
    long long findVertexPotential(size_t curr, StreamGraph &graph, Graph::EDGE_DIRECTION mode)
    {
        if (curr == graph.getEndNumber(mode))
        {
            return 0;
        }
        long long result = 0;
        size_t currLevel = makeCurrLevel(level[curr], mode);
        for (size_t i = 0; i < graph.getVertexDeg(curr, mode); i++)
        {
            if (level[graph.getEdgeOnPosition(curr, i, mode)] == currLevel 
                && potentials[graph.getEdgeOnPosition(curr, i, mode)] != -1)
            {
                result += graph.getEdgeWithoutStream(curr, i, mode);
            }
        }
        return result;
    }
    void findPotentials(StreamGraph &graph)
    {
        for (size_t i = 0; i < graph.getNumberOfVertex(); i++)
        {
            if (potentials[i] == -1)
            {
                continue;
            }
            long long potentialIn = findVertexPotential(i, graph, Graph::EDGE_DIRECTION::REVERSE), 
                potentialOut = findVertexPotential(i, graph, Graph::EDGE_DIRECTION::RIGHT);
            if (i == graph.getStartNumber())
            {
                potentials[i] = potentialOut;
            }
            else if (i == graph.getFinishNumber())
            {
                potentials[i] = potentialIn;
            }
            else
            {
                potentials[i] = min(potentialIn, potentialOut);
            }
        }
    }
    size_t findMinPotential(StreamGraph &graph)
    {
        size_t numberOfVertexWithMinPotential = graph.getNumberOfVertex();
        long long currentMinPotential = graph.getInfDiametr();
        for (size_t i = 0; i < graph.getNumberOfVertex(); i++)
        {
            if (potentials[i] >= 0 && potentials[i] < currentMinPotential)
            {
                currentMinPotential = potentials[i];
                numberOfVertexWithMinPotential = i;
            }
        }
        return numberOfVertexWithMinPotential;
    }
    void pushStream(size_t start, StreamGraph &graph, Graph::EDGE_DIRECTION mode)
    {
        if (potentials[start] > 0)
        {
            queue <size_t> que;
            que.push(start);
            vector <long long> extra(graph.getNumberOfVertex(), 0);
            extra[start] = potentials[start];
            while (!que.empty())
            {
                size_t curr = que.front(), currEdge = 0;
                size_t currLevel = makeCurrLevel(level[curr], mode);
                while (extra[curr] > 0)
                {
                    size_t next = graph.getEdgeOnPosition(curr, currEdge, mode);
                    long long adding = min(extra[curr], graph.getEdgeWithoutStream(curr, currEdge, mode));
                    if (potentials[next] >= 0 && level[next] == currLevel)
                    {
                        if (next != graph.getEndNumber(mode))
                        {
                            que.push(next);
                        }
                        extra[next] += adding;
                        extra[curr] -= adding;
                        graph.addStreamToEdge(curr, currEdge, adding, mode);
                    }
                    currEdge++;
                }
                que.pop();
            }
        }
    }
    void pushStreamFront(size_t start, StreamGraph &graph)
    {
        if (start != graph.getFinishNumber())
        {
            pushStream(start, graph, Graph::EDGE_DIRECTION::RIGHT);
        }
    }
    void pushStreamBack(size_t start, StreamGraph &graph)
    {
        if (start != graph.getStartNumber())
        {
            pushStream(start, graph, Graph::EDGE_DIRECTION::REVERSE);
        }
    }
public:
    void findMaxStream(StreamGraph &graph)
    {
        graph.createStreamNet();
        createLevelVector(graph);
        while (level[graph.getFinishNumber()] < graph.getInf())
        {
            potentials.assign(graph.getNumberOfVertex(), graph.getInfDiametr());
            findPotentials(graph);
            size_t curr = findMinPotential(graph);
            while (curr < graph.getNumberOfVertex())
            {
                pushStreamFront(curr, graph);
                pushStreamBack(curr, graph);
                potentials[curr] = -1;
                findPotentials(graph);
                curr = findMinPotential(graph);
            }
            createLevelVector(graph);
            graph.setStreamSize();
        }
        graph.setStreamSize();
    }
};

int main()
{
    in >> n >> m;
    StreamGraph gr(n);
    for (size_t i = 0; i < m; i++)
    {
        size_t a, b;
        long long c;
        in >> a >> b >> c;
        a--;
        b--;
        input.push_back(make_pair(a, gr.getVertexDeg(a, Graph::EDGE_DIRECTION::RIGHT)));
        gr.addEdge(a, b, c);
    }
    StreamFinder sf;
    sf.findMaxStream(gr);
    out << gr.getStreamSize() << endl;
    for (size_t i = 0; i < m; i++)
    {
        out << gr.getEdgeWithStream(input[i].first, input[i].second, Graph::EDGE_DIRECTION::RIGHT) << endl;
    }
    //in.close();
    //out.close();
    return 0;
}
