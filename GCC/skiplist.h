#ifndef SKIP_LIST_H
#define SKIP_LIST_H

#define element_offset(type, member) (u32)(&((type *)0)->member)
#define element_entry(type, member, ptr) (type *)((u32)ptr - element_offset(type, member))
#define element_node_offset(type, node, key) ((int)(&((type *)0)->key) - (int)(&((type *)0)->node))
#define element_node_key(node, offset) *(int *)((int)node + offset)

#define MAX_LEVEL 5

typedef struct skiplist_node_t
{
  struct skiplist_node_t *nexts[MAX_LEVEL];
} skiplist_node_t;

static const double probability = 0.5;

typedef struct skiplist_t
{
  skiplist_node_t head;
  int size;
  int max_level;
} skiplist_t;

void skip_list_init(skiplist_t *list);
void skip_list_put(skiplist_t *list, skiplist_node_t *node, int offset);
void skip_list_remove(skiplist_t *list, skiplist_node_t *node, int offset);
skiplist_node_t *skip_list_first_node(skiplist_t *list);
skiplist_node_t *skip_list_last_node(skiplist_t *list);
void skip_list_print_all(skiplist_t *list, int offset);

#endif
