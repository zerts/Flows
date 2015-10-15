#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <fstream>

//#define in cin
//#define out cout

using namespace std;

ifstream in("input.txt");
ofstream out("output.txt");

class Edge
{
public:
    size_t to, reverse;
    long long diametr;
    Edge(size_t t, long long d, size_t r)
    {
        to = t;
        diametr = d;
        reverse = r;
    }
};

class Graph
{
protected:
    size_t vertexNumber;
    vector < vector < Edge > > edge;
public:
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
            edge[start].push_back(Edge(finish, diametr, edge[finish].size()));
            edge[finish].push_back(Edge(start, 0, edge[start].size() - 1));
        }
    }
    size_t getVertexDeg(size_t curr)
    {
        return edge[curr].size();
    }
    long long getEdge(size_t start, size_t finish)
    {
        for (size_t i = 0; i < edge[start].size(); i++)
        {
            if (edge[start][i].to == finish)
            {
                return edge[start][i].diametr;
            }
        }
        return 0u;
    }
    size_t getEdgeOnPosition(size_t curr, size_t position)
    {
        if (position < edge[curr].size())
        {
            return edge[curr][position].to;
        }
        return 0;
    }
    long long getDiametrOfEdgeOnPosition(size_t curr, size_t position)
    {
        if (position < edge[curr].size())
        {
            return edge[curr][position].diametr;
        }
        return 0;
    }
    size_t getNumberOfVertex()
    {
        return vertexNumber;
    }
};

class StreamGraph : public Graph
{
private:
    size_t start, finish;
    long long streamSize;
    vector < vector < long long > > edgeWithStream;
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
            edgeWithStream[i].assign(edge[i].size(), 0);
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
    long long getEdgeWithStream(size_t curr, size_t position)
    {
        return edgeWithStream[curr][position];
    }
    long long getEdgeWithoutStream(size_t curr, size_t position)
    {
        return edge[curr][position].diametr - edgeWithStream[curr][position];
    }
    void addStreamToEdge(size_t curr, size_t position, long long adding)
    {
        edgeWithStream[curr][position] += adding;
        edgeWithStream[getEdgeOnPosition(curr, position)][edge[curr][position].reverse] -= adding;
    }
    void setStreamSize()
    {
        streamSize = 0;
        for (size_t i = 0; i < edge[start].size(); i++)
        {
            streamSize += edgeWithStream[start][i];
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
            for (size_t j = 0; j < edge[i].size(); j++)
            {
                if (edgeWithStream[i][j] > 0)
                {
                    out << i << ' ' << getEdgeOnPosition(i, j) << ' ' << edgeWithStream[i][j] << endl;
                }
            }
        }
    }
};

class StreamFinder
{
private:
    vector <size_t> high, currentEdge;
    vector <long long> extra;
    queue<size_t> que;
    void addToQue(size_t curr, StreamGraph &graph)
    {
        if (curr != graph.getFinishNumber() && extra[curr] == 0)
        {
            que.push(curr);
        }
    }
    void push(size_t curr, size_t position, StreamGraph &graph)
    {
        long long streamToEdge = min(graph.getEdgeWithoutStream(curr, position), extra[curr]);
        graph.addStreamToEdge(curr, position, streamToEdge);
        extra[curr] -= streamToEdge;
        addToQue(graph.getEdgeOnPosition(curr, position), graph);
        extra[graph.getEdgeOnPosition(curr, position)] += streamToEdge;
    }
    void relabel(size_t curr, StreamGraph &graph)
    {
        size_t newHigh = graph.getNumberOfVertex() * 3;
        for (size_t i = 0; i < graph.getVertexDeg(curr); i++)
        {
            size_t currNeighbour = graph.getEdgeOnPosition(curr, i);
            if (high[curr] <= high[currNeighbour])
            {
                newHigh = min(newHigh, high[currNeighbour]);
            }
        }
        high[curr] = newHigh + 1;
    }
    void discharge(size_t curr, StreamGraph &graph)
    {
        if (curr != graph.getStartNumber())
        {
            while (extra[curr])
            {
                size_t currEdgeNumber = currentEdge[curr];
                if (currEdgeNumber == graph.getVertexDeg(curr))
                {
                    relabel(curr, graph);
                    currentEdge[curr] = 0;
                }
                else if (graph.getEdgeWithoutStream(curr, currentEdge[curr]) > 0 && 
                    high[curr] == high[graph.getEdgeOnPosition(curr, currentEdge[curr])] + 1)
                {
                    push(curr, currentEdge[curr], graph);
                }
                else
                {
                    currentEdge[curr]++;
                }
            }
        }
    }
public:
    void findMaxStream(StreamGraph &graph)
    {
        high.assign(graph.getNumberOfVertex(), 0u);
        extra.assign(graph.getNumberOfVertex(), 0u);
        currentEdge.assign(graph.getNumberOfVertex(), 0u);
        graph.createStreamNet();
        high[graph.getStartNumber()] = graph.getNumberOfVertex();
        for (size_t i = 0; i < graph.getVertexDeg(graph.getStartNumber()); i++)
        {
            addToQue(graph.getEdgeOnPosition(graph.getStartNumber(), i), graph);

            extra[graph.getEdgeOnPosition(graph.getStartNumber(), i)] +=
                graph.getDiametrOfEdgeOnPosition(graph.getStartNumber(), i);

            graph.addStreamToEdge(graph.getStartNumber(), i, 
                graph.getDiametrOfEdgeOnPosition(graph.getStartNumber(), i));
        }
        while (!que.empty())
        {
            size_t curr = que.front();
            que.pop();
            discharge(curr, graph);
        }
        graph.setStreamSize(extra[graph.getFinishNumber()]);
    }
};

int main()
{
    size_t n, m;
    in >> n >> m;
    StreamGraph gr(n);
    vector < pair < size_t, size_t> > input;
    for (size_t i = 0; i < m; i++)
    {
        size_t a, b;
        long long c;
        in >> a >> b >> c;
        a--;
        b--;
        input.push_back(make_pair(a, gr.getVertexDeg(a)));
        gr.addEdge(a, b, c);
    }
    StreamFinder sf;
    sf.findMaxStream(gr);
    out << gr.getStreamSize() << endl;
    for (size_t i = 0; i < m; i++)
    {
        out << i << ' ' << gr.getEdgeWithStream(input[i].first, input[i].second) << endl;
    }
    //gr.printStreamNet();
    //in.close();
    //out.close();
    return 0;
}
