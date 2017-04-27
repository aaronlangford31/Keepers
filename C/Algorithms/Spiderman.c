#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#define OP_BASE '\0'
#define OP_DOWN 'D'
#define OP_UP   'U'

typedef struct {
  void* prev;
  void* up;
  void* down;
  char op_type;
  int height;
  int max_observed_height;
} node;

typedef struct {
  char valid;
  short max_observed_height;
} cache_item;

node* solution;
cache_item* cache;

void go_up(short ops[], node* prev, int cum_sum[], int index, int num_ops);
void go_down(short ops[], node* prev, int cum_sum[], int index, int num_ops);

void cache_write(int index, int prev_height, int curr_height, int num_ops, cache_item val){
  cache[index + (num_ops * (prev_height + curr_height))] = val;
}

cache_item cache_hit(int index, int prev_height, int curr_height, int num_ops) {
  return cache[index + (num_ops * (prev_height + curr_height))];
}

void go_up(short ops[], node* prev, int cum_sum[], int index, int num_ops) {
  cache_item item = cache_hit(index, prev->height, ops[index], num_ops);
  if(item.valid && item.max_observed_height <= prev->max_observed_height) {
    return;
  } else {
    item.valid = 1;
    item.max_observed_height = prev->max_observed_height;
    cache_write(index, prev->height, ops[index], num_ops, item); 
  }
  
  // IF spiderman is too high, return no solution
  if(cum_sum[index + 1] < prev->height + ops[index]) {
    return;
  }
  if(solution->max_observed_height <= prev->height + ops[index]) {
    return;
  }
  
  // create a node that represents the solution at this state
  // and add it to the solution tree.
  node* this_node = malloc(sizeof(node));
  prev->up = (void*)this_node;
  this_node->prev = (void*)prev;
  this_node->op_type = OP_UP;
  
  // compute the height for this state, and adjust max observed
  // height if necessary.
  this_node->height = prev->height + ops[index];
  this_node->max_observed_height = prev->max_observed_height;
  if(this_node->max_observed_height < this_node->height) {
    this_node->max_observed_height = this_node->height;
  }
  
  // if this is the last node, we need to see if we can get back
  // down to zero.
  if(index == num_ops - 1 && this_node->height == 0) {
    if(solution->max_observed_height > this_node->max_observed_height) {
      solution = this_node;
    }
    return;
  } else if (index == num_ops - 1) {
    return;
  }

  go_up(ops, this_node, cum_sum, index + 1, num_ops);
  go_down(ops, this_node, cum_sum, index + 1, num_ops);
}

void go_down(short ops[], node* prev, int cum_sum[], int index, int num_ops) {
  if(cum_sum[index] < prev->height) {
    return;
  }
  if(prev->height < ops[index]) {
    return;
  }

  cache_item item = cache_hit(index, prev->height, 0-ops[index], num_ops);
  if(item.valid && item.max_observed_height <= prev->max_observed_height) {
    return;
  } else {
    item.valid = 1;
    item.max_observed_height = prev->max_observed_height;
    cache_write(index, prev->height, 0-ops[index], num_ops, item); 
  }

  // create a node that represents the solution at this state
  // and add it to the solution tree.
  node* this_node = malloc(sizeof(node));
  prev->down = (void*)this_node;
  this_node->prev = (void*)prev;
  this_node->op_type = OP_DOWN;

  // compute the height for this state, and copy max height
  this_node->height = prev->height - ops[index];
  this_node->max_observed_height = prev->max_observed_height;

  // if this is the last node, we need to see if we can get back
  // down to zero.
  if(index == num_ops - 1 && this_node->height == 0) {
    if(solution->max_observed_height > this_node->max_observed_height) {
      solution = this_node;
    }
    return;
  } else if (index == num_ops - 1) {
    return;
  }

  go_up(ops, this_node, cum_sum, index + 1, num_ops);
  go_down(ops, this_node, cum_sum, index +1, num_ops);
}

void compute_optimal(short ops[], short num_ops, char result[]) {
  node base;
  base.prev = NULL;
  base.up = NULL;
  base.down = NULL;
  base.op_type = OP_BASE;
  base.height = 0;
  base.max_observed_height = 0;

  node sltn;
  sltn.max_observed_height = 1001;
  sltn.op_type = OP_BASE;
  solution = &sltn;

  cache = malloc(sizeof(cache_item)*1000*num_ops);
  int i;
  for(i = 0; i < 1000*num_ops; i++) {
    cache[i].valid = 0;
  }

  int cum_sum[41];
  cum_sum[num_ops] = 0;
  for(i = num_ops-1; i >= 0; i--) {
    cum_sum[i] =  cum_sum[i + 1] + ops[i]; 
  }

  go_up(ops, &base, cum_sum, 0, num_ops);
  go_down(ops, &base, cum_sum, 0, num_ops);

  if(solution->op_type == OP_BASE) {
    sprintf(result, "IMPOSSIBLE");
    return;
  }

  node* curr = solution;
  result[num_ops] = '\0';
  for(i = num_ops - 1; i >= 0; i--) {
    result[i] = curr->op_type;
    curr = (node*)curr->prev;
  }
  return;
}


int main() {
  uint8_t num_tests;
  scanf("%hhi", &num_tests);

  int i;
  for(i = 0; i < num_tests; i++) {
    short num_ops, ops[40];
    scanf("%hi", &num_ops);
    int j;
    for(j = 0; j < num_ops; j++){
      scanf("%hi", &ops[j]);
    }

    char result[41];
    compute_optimal(ops, num_ops, result);
    printf("%s\n", result);
  }
  return 0;
}
