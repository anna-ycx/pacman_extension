#ifndef GRAPH_H
#define GRAPH_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <raylib.h>

#define HEIGHT 31
#define WIDTH 28

#define NODE_CHARS "-.pgad"

typedef uint16_t nid_t;

//a moveable position on the board
typedef struct node {
  int x;
  int y;
  nid_t id;
} node_t;


//all moveable positions on the board
typedef struct graph {
	node_t **nodes;
  uint16_t size;
}	graph_t;

//given a node position, gets index in array
//return index of value if it is in the array, -1 if not
node_t *search(graph_t *graph, int x, int y);

//return pointer to new node if move is valid, null if not
node_t *try_move(graph_t *graph, node_t *current, Vector2 direction);

node_t *next_in_path(graph_t *graph, nid_t **table, node_t *current, node_t *target);

bool is_adjacent(node_t *n1, node_t *n2);

//for each pair from j to i, set table[j][i]:
// - if j = i, set to 0
// - if i != j and arc exists between i and j set it to 1
// - else set to high number (functionally infinite)
uint8_t **initialise_length_table(graph_t *graph);

//for each pair from j to i, set table[j][i]:
// - if j = i, set to j
// - if i != j and an arc exists between i and j set it to i
// - else set to 0
nid_t **initialise_routing_table(graph_t *graph);

//return all pair shortest path routing table for graph
nid_t **generate(graph_t *graph);

graph_t *populate(char **grid, int no_nodes);

int count_nodes(char **grid);

void free_graph(graph_t *graph);

void print_table(nid_t **table, int size);

#endif