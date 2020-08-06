#include "graphs.h"

//TODO: change to binary search
//given a node position, gets pointer to node in array
//return index of value if it is in the array, -1 if not
node_t *search(graph_t *graph, int x, int y) {
  for (int i = 0; i < graph->size; i++) {
      if (graph->nodes[i]->x == x &&
        graph->nodes[i]->y == y) {
      return graph->nodes[i];
    }
  }
  return NULL;
}


//will always be able to wrap if has a coordinate at bounds AND is moving away from centre
//can wrap in both x and y
node_t *wrap(graph_t *graph, node_t *current, Vector2 direction) {
  //if on left edge
  if (current->x == 0 && direction.x == -1) {
    return search(graph, WIDTH - 1, current->y);
  //if on top edge
  } else if (current->y == 0 && direction.y == -1) {
    return search(graph, current->x, HEIGHT - 1);
  //if on left edge
  } else if (current->x == WIDTH - 1 && direction.x == 1) {
    return search(graph, 0, current->y);
  //if on bottom edge
  } else if (current->y == HEIGHT - 1 && direction.y == 1) {
    return search(graph, current->x, 0);
  }
  return 0;
}


//return pointer to new node if move is valid, null if not
node_t *try_move(graph_t *graph, node_t *current, Vector2 direction) {
  //check if can wrap around screen
  node_t *try_wrap = wrap(graph, current, direction);
  if (try_wrap) {
    return try_wrap;
  //otherwise search graph normally
  } else {
     return search(graph, current->x + direction.x, current->y + direction.y);
  }
}

node_t *next_in_path(graph_t *graph, nid_t **table, node_t *current, node_t *target) {
  return graph->nodes[table[current->id][target->id]];
}

bool is_adjacent(node_t *n1, node_t *n2) {
  return ((n1->y == n2->y) && 
           abs(n1->x - n2->x) == 1) ||
          (n1->x == n2->x && 
         abs(n1->y - n2->y) == 1);
}

//for each pair from j to i, set table[j][i]:
// - if j = i, set to 0
// - if i != j and arc exists between i and j set it to 1
// - else set to high number (functionally infinite)
uint8_t **initialise_length_table(graph_t *graph) {
  uint8_t **table = malloc(graph->size * sizeof(uint8_t *));

  for (int j = 0; j < graph->size; j++) {
    uint8_t *row = malloc(graph->size * sizeof(uint8_t));
    for (int i = 0; i < graph->size; i++) {
      if (i == j) {
        row[i] = 0;
      } else if (is_adjacent(graph->nodes[i], graph->nodes[j])) {
        row[i] = 1;
      } else {
        row[i] = 0xff;
      }
    }
    table[j] = row;
  }
  return table;
}

//for each pair from j to i, set table[j][i]:
// - if j = i, set to j
// - if i != j and an arc exists between i and j set it to i
// - else set to 0
nid_t **initialise_routing_table(graph_t *graph) {
  nid_t **table = malloc(graph->size * sizeof(nid_t *));

  for (int j = 0; j < graph->size; j++) {
    nid_t *row = malloc(graph->size * sizeof(nid_t));
    for (int i = 0; i < graph->size; i++) {
      if (i == j) {
        row[i] = j;
      } else if (is_adjacent(graph->nodes[i], graph->nodes[j])) {
        row[i] = i;
      } else {
        row[i] = 0;
      }
    }
    table[j] = row;
  }
  return table;
}


//return all pair shortest path routing table for graph
nid_t **generate(graph_t *graph) {
  uint8_t **lengths = initialise_length_table(graph);
  nid_t **routes = initialise_routing_table(graph);

  for (int k = 0; k < graph->size; k++) {
    for (int j = 0; j < graph->size; j++) {
      for (int i = 0; i < graph->size; i++) {
        //if a shorter path exists i to j through node k
        if (lengths[i][k] + lengths[k][j] < lengths[i][j]) {
          //update the new shortest length 
          lengths[i][j] = lengths[i][k] + lengths[k][j];
          //update the new next node in shortest path
          routes[i][j] = routes[i][k];
        }
      }
    }
  }
  return routes;
}

graph_t *populate(char **grid, int no_nodes) {
	graph_t *graph = malloc(sizeof(graph));
  node_t **nodes = malloc(no_nodes * sizeof(node_t));
  graph->size = no_nodes;
	int node_count = 0;
	for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
      //if current character is a moveable
      char *ptr = strchr(NODE_CHARS, grid[y][x]);
			if (ptr != NULL) {
        node_t *new_node = malloc(sizeof(node_t));
        new_node->x = x;
        new_node->y = y;
        new_node->id = node_count;
        nodes[node_count] = new_node;
        node_count++;
			}
		}
	}
  graph->nodes = nodes;
  return graph;
}

int count_nodes(char **grid) {
  int count = 0;
  for (int y = 0; y < HEIGHT; y++) {
		for (int x = 0; x < WIDTH; x++) {
      char *ptr = strchr(NODE_CHARS, grid[y][x]);
			if (ptr != NULL) {
        count++;
      }
    }
  }
  return count;
}

void free_graph(graph_t *graph) {
  for (int i = 0; i < graph->size; i++) {
    free(graph->nodes[i]);
  }
	free(graph->nodes);
  free(graph);
}


void print_table(nid_t **table, int size) {
  for (int j = 0; j < size; j++) {
    for (int i = 0; i < size; i++) {
      printf("%2d ", table[j][i]);
    }
    printf("\n");
  }
}


/*
int main(int argc, char**argv) {

  //actual is 31
  char **grid = malloc(HEIGHT * sizeof(char *));
  grid[0] = strdup("xxxxxxxxxxxxxxxxxxxxxxxxxxxx");
	grid[1] = strdup("x------------xx------------x");
  grid[2] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[3] = strdup("xpxxxx-xxxxx-xx-xxxxx-xxxxpx");
  grid[4] = strdup("x-xxxx-xxxxx-xx-xxxxx-xxxx-x");
  grid[5] = strdup("x--------------------------x");
  grid[6] = strdup("x-xxxx-xx-xxxxxxxx-xx-xxxx-x");
  grid[7] = strdup("x-xxxx-xx-xxxxxxxx-xx-xxxx-x");
  grid[8] = strdup("x------xx----xx----xx------x");
  grid[9] = strdup("xxxxxx-xxxxx-xx-xxxxx-xxxxxx");
  grid[10] = strdup("xxxxxx-xxxxx-xx-xxxxx-xxxxxx");
  grid[10] = strdup("xxxxxx-xx..........xx-xxxxxx");
  grid[11] = strdup("xxxxxx-xx.xxxxxxxx.xx-xxxxxx");

	graph_t *graph = populate(grid, count_nodes(grid));
  printf("%d\n", count_nodes(grid));
  nid_t **table = generate(graph);
  print_table(table, graph->size);
  free_graph(graph);
}
*/