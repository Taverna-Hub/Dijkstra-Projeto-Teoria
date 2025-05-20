#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>

#define INF INT_MAX

typedef struct {
    int dest;
    int weight;
} Edge;

typedef struct {
    Edge* edges;
    int edge_count;
    int edge_capacity;
} AdjList;

typedef struct {
    AdjList* vertices;
    int n;
} Graph;

typedef struct {
    int* nodes;
    int* dist;
    int size;
} MinHeap;

void init_graph(Graph* g, int n) {
    g->n = n;
    g->vertices = calloc(n, sizeof(AdjList));
}

void add_edge(Graph* g, int u, int v, int w) {
    AdjList* list = &g->vertices[u];
    if(list->edge_count == list->edge_capacity) {
        list->edge_capacity = list->edge_capacity ? list->edge_capacity * 2 : 4;
        list->edges = realloc(list->edges, list->edge_capacity * sizeof(Edge));
    }
    list->edges[list->edge_count++] = (Edge){v, w};
}

void free_graph(Graph* g) {
    for(int i = 0; i < g->n; i++) {
        free(g->vertices[i].edges);
    }
    free(g->vertices);
}

void swap(int* a, int* b) {
    int t = *a; *a = *b; *b = t;
}

void min_heapify(MinHeap* h, int i, int* pos) {
    int smallest = i;
    int l = 2*i + 1;
    int r = 2*i + 2;
    if(l < h->size && h->dist[h->nodes[l]] < h->dist[h->nodes[smallest]])
        smallest = l;
    if(r < h->size && h->dist[h->nodes[r]] < h->dist[h->nodes[smallest]])
        smallest = r;
    if(smallest != i) {
        pos[h->nodes[i]] = smallest;
        pos[h->nodes[smallest]] = i;
        swap(&h->nodes[i], &h->nodes[smallest]);
        min_heapify(h, smallest, pos);
    }
}

int extract_min(MinHeap* h, int* pos) {
    int root = h->nodes[0];
    h->nodes[0] = h->nodes[h->size - 1];
    pos[h->nodes[0]] = 0;
    h->size--;
    min_heapify(h, 0, pos);
    return root;
}

void decrease_key(MinHeap* h, int v, int new_dist, int* pos) {
    int i = pos[v];
    h->dist[v] = new_dist;
    while(i > 0 && h->dist[h->nodes[i]] < h->dist[h->nodes[(i-1)/2]]) {
        int parent = (i-1)/2;
        pos[h->nodes[i]] = parent;
        pos[h->nodes[parent]] = i;
        swap(&h->nodes[i], &h->nodes[parent]);
        i = parent;
    }
}

int* dijkstra(Graph* g, int start) {
    int n = g->n;
    int* dist = malloc(n * sizeof(int));
    int* visited = calloc(n, sizeof(int));
    int* pos = malloc(n * sizeof(int));
    MinHeap heap;
    heap.size = n;
    heap.nodes = malloc(n * sizeof(int));
    heap.dist = malloc(n * sizeof(int));

    for(int i = 0; i < n; i++) {
        dist[i] = INF;
        heap.dist[i] = INF;
        heap.nodes[i] = i;
        pos[i] = i;
    }

    dist[start] = 0;
    heap.dist[start] = 0;
    decrease_key(&heap, start, 0, pos);

    while(heap.size > 0) {
        int u = extract_min(&heap, pos);
        if(dist[u] == INF) break;
        visited[u] = 1;

        for(int i = 0; i < g->vertices[u].edge_count; i++) {
            int v = g->vertices[u].edges[i].dest;
            int w = g->vertices[u].edges[i].weight;

            if(!visited[v] && dist[u] != INF && dist[v] > dist[u] + w) {
                dist[v] = dist[u] + w;
                decrease_key(&heap, v, dist[v], pos);
            }
        }
    }

    free(heap.nodes);
    free(heap.dist);
    free(pos);
    free(visited);

    return dist;
}

double mean(double* arr, int n) {
    double s = 0;
    for(int i = 0; i < n; i++) s += arr[i];
    return s / n;
}

double stddev(double* arr, int n, double m) {
    double s = 0;
    for(int i = 0; i < n; i++) s += (arr[i] - m) * (arr[i] - m);
    return sqrt(s / n);
}

int main() {
    Graph pequeno;
    init_graph(&pequeno, 1000);
    for(int i = 0; i < 999; i++) {
        add_edge(&pequeno, i, i+1, 1);
        add_edge(&pequeno, i+1, i, 1);
    }

    Graph medio;
    init_graph(&medio, 10000);
    for(int i = 0; i < 9999; i++) {
        add_edge(&medio, i, i+1, 1);
        add_edge(&medio, i+1, i, 1);
    }

    Graph grande;
    init_graph(&grande, 100000);
    for(int i = 0; i < 99999; i++) {
        add_edge(&grande, i, i+1, 1);
        add_edge(&grande, i+1, i, 1);
    }

    Graph grafos[] = {pequeno, medio, grande};
    const char* nomes[] = {"🟢 Pequeno", "🟡 Médio", "🔴 Grande"};
    int rep = 30; 

    for (int g = 0; g < 3; ++g) {
        printf("🚀 Executando Dijkstra no grafo %s (n = %d)\n", nomes[g], grafos[g].n);
        double* tempos = malloc(rep * sizeof(double));
        double t_max = 0.0, t_min = 1e9, t_total = 0.0;

        for (int i = 0; i < rep; i++) {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);

            int* dist = dijkstra(&grafos[g], 0);

            clock_gettime(CLOCK_MONOTONIC, &end);

            free(dist);

            tempos[i] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)/1e9;
            printf("  🔄 Repetição %2d: %.6f segundos\n", i+1, tempos[i]);

            if(tempos[i] > t_max) t_max = tempos[i];
            if(tempos[i] < t_min) t_min = tempos[i];
            t_total += tempos[i];
        }

        double m = mean(tempos, rep);
        double s = stddev(tempos, rep, m);
        printf("📊 Tempo médio: %.6f s, Desvio padrão: %.6f s\n", m, s);
        printf("⏱️ Tempo máximo: %.6f s, Tempo mínimo: %.6f s, Tempo total: %.6f s\n\n", t_max, t_min, t_total);

        free(tempos);
        free_graph(&grafos[g]);
    }

    return 0;
}
