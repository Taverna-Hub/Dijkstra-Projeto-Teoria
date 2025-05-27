#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <time.h>
#include <math.h>
#include "cJSON.h"

#define INF INT_MAX

typedef struct
{
    int dest;
    int weight;
} Edge;

typedef struct
{
    Edge *edges;
    int edge_count;
    int edge_capacity;
} AdjList;

typedef struct
{
    AdjList *vertices;
    int n;
} Graph;

typedef struct
{
    int *nodes;
    int *dist;
    int size;
} MinHeap;

void init_graph(Graph *g, int n)
{
    g->n = n;
    g->vertices = calloc(n, sizeof(AdjList));
}

void add_edge(Graph *g, int u, int v, int w)
{
    AdjList *list = &g->vertices[u];
    if (list->edge_count == list->edge_capacity)
    {
        list->edge_capacity = list->edge_capacity ? list->edge_capacity * 2 : 4;
        list->edges = realloc(list->edges, list->edge_capacity * sizeof(Edge));
    }
    list->edges[list->edge_count++] = (Edge){v, w};
}

void free_graph(Graph *g)
{
    for (int i = 0; i < g->n; i++)
    {
        free(g->vertices[i].edges);
    }
    free(g->vertices);
}

void swap(int *a, int *b)
{
    int t = *a;
    *a = *b;
    *b = t;
}

void min_heapify(MinHeap *h, int i, int *pos)
{
    int smallest = i;
    int l = 2 * i + 1;
    int r = 2 * i + 2;
    if (l < h->size && h->dist[h->nodes[l]] < h->dist[h->nodes[smallest]])
        smallest = l;
    if (r < h->size && h->dist[h->nodes[r]] < h->dist[h->nodes[smallest]])
        smallest = r;
    if (smallest != i)
    {
        pos[h->nodes[i]] = smallest;
        pos[h->nodes[smallest]] = i;
        swap(&h->nodes[i], &h->nodes[smallest]);
        min_heapify(h, smallest, pos);
    }
}

int extract_min(MinHeap *h, int *pos)
{
    int root = h->nodes[0];
    h->nodes[0] = h->nodes[h->size - 1];
    pos[h->nodes[0]] = 0;
    h->size--;
    min_heapify(h, 0, pos);
    return root;
}

void decrease_key(MinHeap *h, int v, int new_dist, int *pos)
{
    int i = pos[v];
    h->dist[v] = new_dist;
    while (i > 0 && h->dist[h->nodes[i]] < h->dist[h->nodes[(i - 1) / 2]])
    {
        int parent = (i - 1) / 2;
        pos[h->nodes[i]] = parent;
        pos[h->nodes[parent]] = i;
        swap(&h->nodes[i], &h->nodes[parent]);
        i = parent;
    }
}

int *dijkstra(Graph *g, int start)
{
    int n = g->n;
    int *dist = malloc(n * sizeof(int));
    int *visited = calloc(n, sizeof(int));
    int *pos = malloc(n * sizeof(int));
    MinHeap heap;
    heap.size = n;
    heap.nodes = malloc(n * sizeof(int));
    heap.dist = malloc(n * sizeof(int));

    for (int i = 0; i < n; i++)
    {
        dist[i] = INF;
        heap.dist[i] = INF;
        heap.nodes[i] = i;
        pos[i] = i;
    }

    dist[start] = 0;
    heap.dist[start] = 0;
    decrease_key(&heap, start, 0, pos);

    while (heap.size > 0)
    {
        int u = extract_min(&heap, pos);
        if (dist[u] == INF)
            break;
        visited[u] = 1;

        for (int i = 0; i < g->vertices[u].edge_count; i++)
        {
            int v = g->vertices[u].edges[i].dest;
            int w = g->vertices[u].edges[i].weight;

            if (!visited[v] && dist[u] != INF && dist[v] > dist[u] + w)
            {
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

double mean(double *arr, int n)
{
    double s = 0;
    for (int i = 0; i < n; i++)
        s += arr[i];
    return s / n;
}

double stddev(double *arr, int n, double m)
{
    double s = 0;
    for (int i = 0; i < n; i++)
        s += (arr[i] - m) * (arr[i] - m);
    return sqrt(s / n);
}

// Função utilitária para ler todo o conteúdo de um arquivo para um buffer
char *read_file_to_buffer(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    if (!f)
        return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char *buf = malloc(len + 1);
    if (!buf)
    {
        fclose(f);
        return NULL;
    }
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

// Carrega grafo no formato node-link do NetworkX
static int id_to_index(int *id_to_idx, int n, int id)
{
    for (int i = 0; i < n; i++)
        if (id_to_idx[i] == id)
            return i;
    return -1;
}

int load_graph_from_json(const char *filename, Graph *g)
{
    char *buffer = read_file_to_buffer(filename);
    if (!buffer)
        return 0;
    cJSON *root = cJSON_Parse(buffer);
    free(buffer);
    if (!root)
        return 0;

    cJSON *nodes = cJSON_GetObjectItem(root, "nodes");
    cJSON *links = cJSON_GetObjectItem(root, "links");
    if (!cJSON_IsArray(nodes) || !cJSON_IsArray(links))
    {
        cJSON_Delete(root);
        return 0;
    }
    int n = cJSON_GetArraySize(nodes);
    init_graph(g, n);

    int *id_to_idx = malloc(sizeof(int) * n);
    for (int i = 0; i < n; i++)
    {
        cJSON *node = cJSON_GetArrayItem(nodes, i);
        int id = cJSON_GetObjectItem(node, "id")->valueint;
        id_to_idx[i] = id;
    }
    for (int i = 0; i < cJSON_GetArraySize(links); i++)
    {
        cJSON *link = cJSON_GetArrayItem(links, i);
        int src = cJSON_GetObjectItem(link, "source")->valueint;
        int tgt = cJSON_GetObjectItem(link, "target")->valueint;
        int w = 1;
        cJSON *weight = cJSON_GetObjectItem(link, "weight");
        if (weight && cJSON_IsNumber(weight))
            w = weight->valueint;
        int u = id_to_index(id_to_idx, n, src);
        int v = id_to_index(id_to_idx, n, tgt);
        if (u >= 0 && v >= 0)
            add_edge(g, u, v, w);
    }
    free(id_to_idx);
    cJSON_Delete(root);
    return 1;
}

void limpar_terminal()
{
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void mostrar_menu()
{
    printf("\nEscolha o grafo para executar o algoritmo de Dijkstra:\n");
    printf("[0] Sair do programa\n");
    printf("[1] Grafo Pequeno (Melhor caso)\n");
    printf("[2] Grafo Pequeno (Caso médio)\n");
    printf("[3] Grafo Pequeno (Pior caso)\n");
    printf("[4] Grafo Médio (Melhor caso)\n");
    printf("[5] Grafo Médio (Caso médio)\n");
    printf("[6] Grafo Médio (Pior caso)\n");
    printf("[7] Grafo Grande (Melhor caso)\n");
    printf("[8] Grafo Grande (Caso médio)\n");
    printf("[9] Grafo Grande (Pior caso)\n");
    printf("Escolha (0-9): ");
}

int main()
{
    while (1)
    {
        int opcao;
        Graph g;
        const char *arquivo;
        const char *nome_grafo;

        mostrar_menu();
        scanf("%d", &opcao);
        limpar_terminal();

        if (opcao == 0)
        {
            printf("Programa finalizado!\n");
            break;
        }

        switch (opcao)
        {
        case 1:
            arquivo = "Graphs/grafo_P_melhor.json";
            nome_grafo = "🟢 Pequeno (Melhor caso)";
            break;
        case 2:
            arquivo = "Graphs/grafo_P_medio.json";
            nome_grafo = "🟢 Pequeno (Caso médio)";
            break;
        case 3:
            arquivo = "Graphs/grafo_P_pior.json";
            nome_grafo = "🟢 Pequeno (Pior caso)";
            break;
        case 4:
            arquivo = "Graphs/grafo_M_melhor.json";
            nome_grafo = "🟡 Médio (Melhor caso)";
            break;
        case 5:
            arquivo = "Graphs/grafo_M_medio.json";
            nome_grafo = "🟡 Médio (Caso médio)";
            break;
        case 6:
            arquivo = "Graphs/grafo_M_pior.json";
            nome_grafo = "🟡 Médio (Pior caso)";
            break;
        case 7:
            arquivo = "Graphs/grafo_G_melhor.json";
            nome_grafo = "🔴 Grande (Melhor caso)";
            break;
        case 8:
            arquivo = "Graphs/grafo_G_medio.json";
            nome_grafo = "🔴 Grande (Caso médio)";
            break;
        case 9:
            arquivo = "Graphs/grafo_G_pior.json";
            nome_grafo = "🔴 Grande (Pior caso)";
            break;
        default:
            printf("Opção inválida!\n");
            continue;
        }

        if (!load_graph_from_json(arquivo, &g))
        {
            fprintf(stderr, "Erro ao carregar o grafo %s\n", nome_grafo);
            continue;
        }

        int rep = 30;
        printf("🚀 Executando Dijkstra no grafo %s (n = %d)\n", nome_grafo, g.n);
        double *tempos = malloc(rep * sizeof(double));
        double t_max = 0.0, t_min = 1e9, t_total = 0.0;

        for (int i = 0; i < rep; i++)
        {
            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);

            int *dist = dijkstra(&g, 0);

            clock_gettime(CLOCK_MONOTONIC, &end);
            free(dist);

            tempos[i] = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
            printf("  🔄 Repetição %2d: %.6f segundos\n", i + 1, tempos[i]);

            if (tempos[i] > t_max)
                t_max = tempos[i];
            if (tempos[i] < t_min)
                t_min = tempos[i];
            t_total += tempos[i];
        }

        double m = mean(tempos, rep);
        double s = stddev(tempos, rep, m);
        printf("📊 Tempo médio: %.6f s, Desvio padrão: %.6f s\n", m, s);
        printf("⏱️ Tempo máximo: %.6f s, Tempo mínimo: %.6f s, Tempo total: %.6f s\n\n", t_max, t_min, t_total);

        free(tempos);
        free_graph(&g);

        printf("\nPressione Enter para continuar...");
        while (getchar() != '\n')
            ;      // Limpa o buffer
        getchar(); // Espera pelo Enter
        limpar_terminal();
    }

    return 0;
}
