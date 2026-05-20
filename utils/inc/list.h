#ifndef LIST_H
#define LIST_H

#include "task_cb.h"
typedef struct {
  int num_elements;
  TaskCB_t * root;
  TaskCB_t * tail;
} List_t;

List_t * new_list();

int delete_list();

int append_to_end_list(List_t * list, TaskCB_t * task);

int insert_front(List_t * list, TaskCB_t * task);

int delete_from_list(TaskCB_t * task);

int insert_at_index(List_t * list, int index);

#endif // LIST_H

