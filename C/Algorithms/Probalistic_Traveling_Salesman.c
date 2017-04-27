#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define CONF -4.0

#define IS_VISITED(n, i) (1 << i) & n
#define VISIT(n, i) n += (1 << i)
#define GET_N_VISITED(n) n >> 24
#define INC_N_VISITED(n) n += 1 << 24

typedef struct graph {
  int* nodes;
  int dimension;
} graph;

int optimum;
graph* g;

void establish_baseline() {
  // establish baseline
  optimum = 0;
  int i = 1;
  while(1) {
     optimum += g->nodes[i];
     if(g->dimension - (i % g->dimension) == 1) {
       break;  
     } else {
       i += g->dimension + 1;
     }
  }
  optimum += g->nodes[i+1];
}

char should_proceed_probabalistic(int path_cost, unsigned int visited) {
  float num_remaining = (float)(g->dimension - (GET_N_VISITED(visited)));
  float num_remaining_avg = (float)(optimum - path_cost)/num_remaining;
  
  if(num_remaining < 3)
    return 1;

  if(num_remaining_avg < 1)
    return 0;

  int sum, c, i, j;
  sum = c = 0;
  float mean, std, sterr;
  
  for(i=1;i<g->dimension;i++) {
    if(IS_VISITED(visited, i)) {
      continue;
    }
    int row = g->dimension * i;
    for(j=0;j<g->dimension;j++) {
      if(i == j) {
        continue;
      }
      if(!j && IS_VISITED(visited, j)){
        continue;
      }
      c++;
      sum += g->nodes[row + j];
    }
  }
  
  mean = (float)sum/(float)c;
  
  for(i=1;i<g->dimension;i++) {
    if(IS_VISITED(visited, i)) {
      continue;
    }
    int row = g->dimension * i;
    for(j=0;j<g->dimension;j++) {
      if(i == j) {
        continue;
      }
      if(!j && IS_VISITED(visited, j)){
        continue;
      }
      std += pow((float)g->nodes[row + j] - mean, 2);
    }
  }
  std = sqrt(std/(float)c);
  sterr = std/sqrt(c);
  
  float z = (num_remaining_avg - mean)/sterr;
  
  if(z > CONF) {
    return 1;
  } else {
    return 0;
  }
}

char should_proceed(int path_cost, unsigned int visited) {
  if(g->dimension > 15)
    return should_proceed_probabalistic(path_cost, visited);
  int i, j, min_possible_cost;

  min_possible_cost = 0;
  for(i=1;i<g->dimension;i++) {
    if(IS_VISITED(visited, i)) {
      continue;
    }
    int row = g->dimension * i;
    int min_edge = 501;
    for(j=0;j<g->dimension;j++) {
      if(i == j) {
        continue;
      }
      if(!j && IS_VISITED(visited, j)){
        continue;
      }
      if(g->nodes[row + j] < min_edge){
        min_edge = g->nodes[row + j];
      }
    }
    min_possible_cost += min_edge;
  }

  if(path_cost + min_possible_cost < optimum) {
    return 1;
  } else {
    return 0;
  }
}

void find(int ix, int path_cost, unsigned int visited) {
  if(g->dimension - (GET_N_VISITED(visited)) == 1) {
    int sln_cost = path_cost + g->nodes[ix * g->dimension];
    if(sln_cost < optimum) {
      optimum = sln_cost;
    }
    return;
  }

  if(!should_proceed(path_cost, visited)) {
    return;
  } 
  
  int i;
  VISIT(visited, ix);
  INC_N_VISITED(visited);

  for(i = 1; i < g->dimension; i++) {
    if(i == ix) {
      continue;
    } else if (IS_VISITED(visited, i)) {
      continue;
    } else {
      int row = ix * g->dimension;
      int t_path_cost = path_cost + g->nodes[row + i];
      if(t_path_cost < optimum) {
        find(i, t_path_cost, visited);
      }
    }
  }
}

int main() {   
  int n,g_p[484];
  scanf("%d", &n);

  graph stack_g;
  g = &stack_g;
  g->nodes = g_p;
  g->dimension = n;

  int i, j;
  for(i = 0; i < n; i++) {
    for(j = 0; j < n; j++) {
      scanf("%d", g->nodes + (i*n) + j);
      if(i == j)
        g->nodes[(i*n) + j] = 0;
    }
  }

  establish_baseline();
  
  find(0, 0, 0);
  printf("%d\n", optimum);
  return 0;
}

