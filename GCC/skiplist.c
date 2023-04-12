#include "skiplist.h"

static u32 hold_rand = 1;
static int rand()
{
  return (((hold_rand = hold_rand * 214013L + 2531011L) >> 16) & 0x7fff);
}

void skip_list_init(skiplist_t *list)
{
  for (int i = 0; i < MAX_LEVEL; i++)
  {
    list->head.nexts[i] = NULL;
  }
  list->size = 0;
  list->max_level = 0;
}

static int compare_key(skiplist_node_t *lhs, skiplist_node_t *rhs, int offset)
{
  int first = element_node_key(lhs, offset);
  int second = element_node_key(rhs, offset);
  if (first > second)
  {
    return 1;
  }
  else if (first < second)
  {
    return -1;
  }
  else
  {
    return 0;
  }
}

// 在第level层里，往右移动
// 现在来到的节点是cur，来到了cur的level层，在level层上，找到 < key 最后一个节点并返回
skiplist_node_t *pre_node_in_level(skiplist_t *list, skiplist_node_t *node, skiplist_node_t *cur, int level, int offset)
{
  skiplist_node_t *next = cur->nexts[level];
  while (next != NULL && compare_key(next, node, offset) < 0)
  {
    cur = next;
    next = cur->nexts[level];
  }
  return cur;
}

// 从最高层开始，一路找下去，
// 最终，找到第0层 < key 的最右的节点
skiplist_node_t *pre_node_in_tree(skiplist_t *list, skiplist_node_t *node, int offset)
{
  if (node == NULL)
  {
    return NULL;
  }

  int level = list->max_level;
  skiplist_node_t *cur = &list->head;
  while (level >= 0)
  {
    // 从上层往下层找
    cur = pre_node_in_level(list, node, cur, level--, offset);
  }
  return cur;
}

// 是否包含这个节点
bool skip_list_contains(skiplist_t *list, skiplist_node_t *node, int offset)
{
  if (node == NULL)
  {
    return true;
  }
  skiplist_node_t *pre = pre_node_in_tree(list, node, offset);
  skiplist_node_t *next = pre->nexts[0];
  while (next != NULL && compare_key(next, node, offset) == 0)
  {
    if (next == node)
    {
      return true;
    }
    next = next->nexts[0];
  }
  return false;
}

// 新增节点
void skip_list_put(skiplist_t *list, skiplist_node_t *node, int offset)
{
  if (node == NULL)
  {
    return;
  }

  list->size++;
  int newNodeLevel = 0;
  while ((rand() % 50 < 25) && newNodeLevel < MAX_LEVEL - 1)
  {
    newNodeLevel++;
  }
  while (newNodeLevel > list->max_level)
  {
    list->max_level++;
  }
  int level = list->max_level;
  skiplist_node_t *pre = &list->head;
  while (level >= 0)
  {
    // 在 level 层中，找到最右的 < key 的节点
    pre = pre_node_in_level(list, node, pre, level, offset);
    if (level <= newNodeLevel)
    {
      // 将两个相邻的节点连接
      node->nexts[level] = pre->nexts[level];
      pre->nexts[level] = node;
    }
    level--;
  }
}

// 删除节点
void skip_list_remove(skiplist_t *list, skiplist_node_t *node, int offset)
{
  if (skip_list_contains(list, node, offset))
  {
    list->size--;
    int level = list->max_level;
    skiplist_node_t *pre = &list->head;
    while (level >= 0)
    {
      pre = pre_node_in_level(list, node, pre, level, offset);
      skiplist_node_t *next = pre->nexts[level];
      while (next != NULL && compare_key(next, node, offset) == 0)
      {
        if (next == node)
        {
          // 从key值相同的节点中，找到地址相同的
          break;
        }
      }
      if (next != NULL && compare_key(next, node, offset) == 0)
      {
        pre->nexts[level] = next->nexts[level];
      }
      // 在level层只有一个节点了，就是默认节点head
      if (level != 0 && pre == &list->head && pre->nexts[level] == NULL)
      {
        // 删除 level
        list->head.nexts[level] = NULL;
        list->max_level--;
      }
      level--;
    }
  }
}

// 返回第一个节点
skiplist_node_t *skip_list_first_node(skiplist_t *list)
{
  return list->head.nexts[0];
}

// 返回最后一个节点
skiplist_node_t *skip_list_last_node(skiplist_t *list)
{
  int level = list->max_level;
  skiplist_node_t *cur = &list->head;
  while (level >= 0)
  {
    skiplist_node_t *next = cur->nexts[level];
    while (next != NULL)
    {
      cur = next;
      next = cur->nexts[level];
    }
    level--;
  }
  return cur;
}

// 打印所有节点
void skip_list_print_all(skiplist_t *list, int offset)
{
  for (int i = list->max_level; i >= 0; i--)
  {
    printf("Level %d:", i);
    skiplist_node_t *cur = &list->head;
    while (cur->nexts[i] != NULL)
    {
      skiplist_node_t *next = cur->nexts[i];
      printf("(%d)", element_node_key(next, offset));
      cur = next;
    }
    printf("\n");
  }
}

typedef struct data_t
{
    int key;
    skiplist_node_t node;
} data_t;

void test_skiplist()
{
    data_t d[30];
    skiplist_t list;
    skip_list_init(&list);
    for (int i = 0; i < 30; i++)
    {
        d[i].key = i;
        skip_list_put(&list, &d[i].node, element_node_offset(data_t, node, key));
    }
    skip_list_print_all(&list, element_node_offset(data_t, node, key));
    skiplist_node_t *first = skip_list_first_node(&list);
    printk("First:%d\n", element_node_key(first, element_node_offset(data_t, node, key)));
    skiplist_node_t *last = skip_list_last_node(&list);
    printk("Last:%d\n", element_node_key(last, element_node_offset(data_t, node, key)));
    for (int i = 29; i >= 0; i--)
    {
        skip_list_remove(&list, &d[i].node, element_node_offset(data_t, node, key));
        skip_list_print_all(&list, element_node_offset(data_t, node, key));
    }
}
