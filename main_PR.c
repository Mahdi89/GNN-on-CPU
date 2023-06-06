#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_NAME_LENGTH 100

typedef struct Node {
    char name[MAX_NAME_LENGTH];
    struct Node** children;
    struct Node** parents;
    float auth;
    float hub;
    float pagerank;
} Node;

typedef struct Graph {
    Node** nodes;
    int num_nodes;
} Graph;

Graph* createGraph() {
    Graph* graph = (Graph*)malloc(sizeof(Graph));
    graph->nodes = NULL;
    graph->num_nodes = 0;
    return graph;
}

int containsNode(Graph* graph, const char* name) {
    int i;
    for (i = 0; i < graph->num_nodes; i++) {
        if (strcmp(graph->nodes[i]->name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

Node* findNode(Graph* graph, const char* name) {
    int i;
    for (i = 0; i < graph->num_nodes; i++) {
        if (strcmp(graph->nodes[i]->name, name) == 0) {
            return graph->nodes[i];
        }
    }
    return NULL;
}

void linkChild(Node* node, Node* new_child) {
    int i;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->children[i] == new_child) {
            return;
        }
    }
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->children[i] == NULL) {
            node->children[i] = new_child;
            return;
        }
    }
}

void linkParent(Node* node, Node* new_parent) {
    int i;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->parents[i] == new_parent) {
            return;
        }
    }
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->parents[i] == NULL) {
            node->parents[i] = new_parent;
            return;
        }
    }
}

void updateAuth(Node* node) {
    int i;
    node->auth = 0.0;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->parents[i] != NULL) {
            node->auth += node->parents[i]->hub;
        }
    }
}

void updateHub(Node* node) {
    int i;
    node->hub = 0.0;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->children[i] != NULL) {
            node->hub += node->children[i]->auth;
        }
    }
}

void updatePagerank(Node* node, float d, int n) {
    int i;
    float pagerank_sum = 0.0;
    int num_in_neighbors = 0;
    for (i = 0; i < MAX_NAME_LENGTH; i++) {
        if (node->parents[i] != NULL) {
            pagerank_sum += node->parents[i]->pagerank / (float)n;
            num_in_neighbors++;
        }
    }
    float random_jumping = d / (float)n;
    node->pagerank = random_jumping + (1.0 - d) * pagerank_sum;
}

void normalizeAuthHub(Graph* graph) {
    int i;
    float auth_sum = 0.0;
    float hub_sum = 0.0;
    for (i = 0; i < graph->num_nodes; i++) {
        auth_sum += graph->nodes[i]->auth;
        hub_sum += graph->nodes[i]->hub;
    }
    for (i = 0; i < graph->num_nodes; i++) {
        graph->nodes[i]->auth /= auth_sum;
        graph->nodes[i]->hub /= hub_sum;
    }
}

void normalizePagerank(Graph* graph) {
    int i;
    float pagerank_sum = 0.0;
    for (i = 0; i < graph->num_nodes; i++) {
        pagerank_sum += graph->nodes[i]->pagerank;
    }
    for (i = 0; i < graph->num_nodes; i++) {
        graph->nodes[i]->pagerank /= pagerank_sum;
    }
}

void display(Graph* graph) {
    int i, j;
    for (i = 0; i < graph->num_nodes; i++) {
        printf("%s links to: ", graph->nodes[i]->name);
        for (j = 0; j < MAX_NAME_LENGTH; j++) {
            if (graph->nodes[i]->children[j] != NULL) {
                printf("%s ", graph->nodes[i]->children[j]->name);
            }
        }
        printf("\n");
    }
}

void displayAuthHub(Graph* graph) {
    int i;
    for (i = 0; i < graph->num_nodes; i++) {
        printf("%s  Auth: %.3f  Hub: %.3f\n", graph->nodes[i]->name, graph->nodes[i]->auth, graph->nodes[i]->hub);
    }
}

void displayPagerank(Graph* graph) {
    int i;
    for (i = 0; i < graph->num_nodes; i++) {
        printf("%.3f ", graph->nodes[i]->pagerank);
    }
    printf("\n");
}

void addEdge(Graph* graph, const char* parent, const char* child) {
    Node* parent_node = findNode(graph, parent);
    Node* child_node = findNode(graph, child);
    if (parent_node == NULL) {
        parent_node = (Node*)malloc(sizeof(Node));
        strncpy(parent_node->name, parent, MAX_NAME_LENGTH);
        parent_node->children = (Node**)calloc(MAX_NAME_LENGTH, sizeof(Node*));
        parent_node->parents = (Node**)calloc(MAX_NAME_LENGTH, sizeof(Node*));
        parent_node->auth = 1.0;
        parent_node->hub = 1.0;
        parent_node->pagerank = 1.0;
        graph->nodes = (Node**)realloc(graph->nodes, (graph->num_nodes + 1) * sizeof(Node*));
        graph->nodes[graph->num_nodes] = parent_node;
        graph->num_nodes++;
    }
    if (child_node == NULL) {
        child_node = (Node*)malloc(sizeof(Node));
        strncpy(child_node->name, child, MAX_NAME_LENGTH);
        child_node->children = (Node**)calloc(MAX_NAME_LENGTH, sizeof(Node*));
        child_node->parents = (Node**)calloc(MAX_NAME_LENGTH, sizeof(Node*));
        child_node->auth = 1.0;
        child_node->hub = 1.0;
        child_node->pagerank = 1.0;
        graph->nodes = (Node**)realloc(graph->nodes, (graph->num_nodes + 1) * sizeof(Node*));
        graph->nodes[graph->num_nodes] = child_node;
        graph->num_nodes++;
    }
    linkChild(parent_node, child_node);
    linkParent(child_node, parent_node);
}

void sortNodes(Graph* graph) {
    int i, j;
    for (i = 0; i < graph->num_nodes - 1; i++) {
        for (j = 0; j < graph->num_nodes - i - 1; j++) {
            if (strcmp(graph->nodes[j]->name, graph->nodes[j + 1]->name) > 0) {
                Node* temp = graph->nodes[j];
                graph->nodes[j] = graph->nodes[j + 1];
                graph->nodes[j + 1] = temp;
            }
        }
    }
}

Graph* initGraph(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Failed to open file.\n");
        return NULL;
    }

    Graph* graph = createGraph();
    char line[256];
    while (fgets(line, sizeof(line), file)) {
        char parent[MAX_NAME_LENGTH], child[MAX_NAME_LENGTH];
        sscanf(line, "%[^,],%s", parent, child);
        addEdge(graph, parent, child);
    }
    sortNodes(graph);
    fclose(file);
    return graph;
}

void pageRankOneIter(Graph* graph, float d) {
    int i;
    for (i = 0; i < graph->num_nodes; i++) {
        updatePagerank(graph->nodes[i], d, graph->num_nodes);
    }
    normalizePagerank(graph);
}

void pageRank(Graph* graph, float d, int iteration) {
    int i;
    for (i = 0; i < iteration; i++) {
        pageRankOneIter(graph, d);
    }
}

void outputPageRank(int iteration, Graph* graph, float damping_factor, const char* result_dir, const char* filename) {
    char pagerank_fname[MAX_NAME_LENGTH] = "_PageRank.txt";

    pageRank(graph, damping_factor, iteration);
    printf("PageRank:\n");
    displayPagerank(graph);
    printf("\n");

    char path[MAX_NAME_LENGTH];
    snprintf(path, sizeof(path), "%s/%s", result_dir, filename);
    //mkdir(path, 0777);

    //FILE* file = fopen(strcat(path, strcat(filename, pagerank_fname)), "w");
    FILE* file = fopen("", "w");
    if (file == NULL) {
        printf("Failed to create output file.\n");
        return;
    }

    for (int i = 0; i < graph->num_nodes; i++) {
        fprintf(file, "%.3f ", graph->nodes[i]->pagerank);
    }
    fclose(file);
}

int main(int argc, char** argv) {
    char* input_file = "graph_1.txt";
    float damping_factor = 0.15;
    float decay_factor = 0.9;
    int iteration = 500;

    char result_dir[] = "result";
    char* filename = strrchr(input_file, '/') + 1;
    filename[strcspn(filename, ".")] = '\0';

    Graph* graph = initGraph(input_file);
    if (graph == NULL) {
        printf("Failed to initialize graph.\n");
        return 1;
    }
    outputPageRank(iteration, graph, damping_factor, result_dir, filename);
    return 0;
}
