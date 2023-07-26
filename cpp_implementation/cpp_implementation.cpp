#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iomanip>

class Node
{
public:
    std::string name;
    std::vector<Node *> children;
    std::vector<Node *> parents;
    double auth = 1.0;
    double hub = 1.0;
    double pagerank = 1.0;

    Node(const std::string &name) : name(name), auth(1.0), hub(1.0), pagerank(1.0) {}

    void linkChild(Node *newChild)
    {
        for (Node *child : children)
        {
            if (child->name == newChild->name)
            {
                return;
            }
        }
        children.push_back(newChild);
    }

    void linkParent(Node *newParent)
    {
        for (Node *parent : parents)
        {
            if (parent->name == newParent->name)
            {
                return;
            }
        }
        parents.push_back(newParent);
    }

    void updateAuth()
    {
        // auth = 0.0;
        for (Node *parent : parents)
        {
            auth += parent->hub;
        }
    }

    void updateHub()
    {
        // hub = 0.0;
        for (Node *child : children)
        {
            hub += child->auth;
        }
    }

    void updatePagerank(double d, int n)
    {
        double pagerankSum = 0.0;
        for (Node *parent : parents)
        {
            pagerankSum += parent->pagerank / parent->children.size();
        }
        double randomJumping = d / n;
        pagerank = randomJumping + (1 - d) * pagerankSum;
    }
};

class Graph
{
public:
    std::vector<Node *> nodes;

    Graph() {}

    bool contains(const std::string &name)
    {
        for (Node *node : nodes)
        {
            if (node->name == name)
            {
                return true;
            }
        }
        return false;
    }

    // Return the node with the name, create and return new node if not found
    Node *find(const std::string &name)
    {
        if (!contains(name))
        {
            Node *new_node = new Node(name);
            nodes.push_back(new_node);
            return new_node;
        }
        else
        {
            for (Node *node : nodes)
            {
                if (node->name == name)
                {
                    return node;
                }
            }
        }
        return nullptr;
    }

    void addEdge(const std::string &parent, const std::string &child)
    {
        Node *parentNode = find(parent);
        Node *childNode = find(child);

        parentNode->linkChild(childNode);
        childNode->linkParent(parentNode);
    }

    void display()
    {
        for (Node *node : nodes)
        {
            std::cout << node->name << " links to [";
            for (Node *child : node->children)
            {
                std::cout << child->name << ", ";
            }
            std::cout << "]" << std::endl;
        }
    }

    void sortNodes()
    {
        std::sort(nodes.begin(), nodes.end(),
                  [](Node *a, Node *b)
                  { return std::stoi(a->name) < std::stoi(b->name); });
    }

    void displayHubAuth()
    {
        for (Node *node : nodes)
        {
            std::cout << node->name << "\tAuth:\t" << node->auth << "\tHub:\t" << node->hub;
        }
    }

    void normalizeAuthHub()
    {
        double authSum = 0.0;
        double hubSum = 0.0;

        for (Node *node : nodes)
        {
            authSum += node->auth;
            hubSum += node->hub;
        }

        for (Node *node : nodes)
        {
            node->auth /= authSum;
            node->hub /= hubSum;
        }
    }

    void normalizePagerank()
    {
        double pagerankSum = 0.0;

        for (Node *node : nodes)
        {
            pagerankSum += node->pagerank;
        }

        for (Node *node : nodes)
        {
            node->pagerank /= pagerankSum;
        }
    }

    std::pair<std::vector<double>, std::vector<double>> getAuthHubList()
    {
        std::vector<double> authList;
        std::vector<double> hubList;

        for (Node *node : nodes)
        {
            authList.push_back(node->auth);
            hubList.push_back(node->hub);
        }

        return std::make_pair(authList, hubList);
    }

    std::vector<double> getPagerankList()
    {
        std::vector<double> pagerankList;

        for (Node *node : nodes)
        {
            pagerankList.push_back(node->pagerank);
        }

        return pagerankList;
    }
};

Graph *initGraph(const std::string &fname)
{
    std::ifstream file(fname);
    std::string line;
    Graph *graph = new Graph();

    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string parent, child;
        std::getline(iss, parent, ',');
        std::getline(iss, child, ',');
        graph->addEdge(parent, child);
    }

    graph->sortNodes();

    return graph;
}

void pageRankOneIter(Graph *graph, double d)
{
    std::vector<Node *> &node_list = graph->nodes;
    for (Node *node : node_list)
    {
        node->updatePagerank(d, graph->nodes.size());
    }
    graph->normalizePagerank();
}

void pageRank(Graph *graph, double d, int iteration = 100)
{
    for (int i = 0; i < iteration; ++i)
    {
        pageRankOneIter(graph, d);
    }
}

void outputPageRank(int iteration, Graph *graph, double damping_factor, const std::string &result_dir, const std::string &fname)
{
    std::string pagerank_fname = "_PageRank.txt";
    pageRank(graph, damping_factor, iteration);
    std::vector<double> pagerank_list = graph->getPagerankList();

    std::cout << "PageRank:" << std::endl;
    for (double pr : pagerank_list)
    {
        std::cout << pr << " ";
    }
    std::cout << std::endl;

    std::string path = result_dir + "/" + fname;
    std::ofstream outfile(path + pagerank_fname);
    if (outfile.is_open())
    {
        for (double pr : pagerank_list)
        {
            outfile << std::fixed << std::setprecision(3) << pr << " ";
        }
        outfile << std::endl;
        outfile.close();
    }
}

int main()
{
    std::string input_file = "dataset/graph_1.txt";
    double damping_factor = 0.15;
    // double decay_factor = 0.9;
    int iteration = 500;

    std::string result_dir = "result";
    std::string fname = input_file.substr(input_file.find_last_of("/") + 1, input_file.find_last_of(".") - input_file.find_last_of("/") - 1);

    Graph *graph = initGraph(input_file);
    outputPageRank(iteration, graph, damping_factor, result_dir, fname);

    // Clean up memory
    for (Node *node : graph->nodes)
    {
        delete node;
    }
    delete graph;

    return 0;
}