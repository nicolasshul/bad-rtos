#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task_cb.h"

typedef struct {
  int num_elements;
  TaskCB_t * root;
  TaskCB_t * tail;
} Task_List_t;

Task_List_t * new_list();

void delete_list(Task_List_t * list);

int append_to_end_list(Task_List_t * list, TaskCB_t * task);

int insert_front_list(Task_List_t * list, TaskCB_t * task);

int remove_from_list(Task_List_t * list, TaskCB_t * task);

int insert_at_index_list(Task_List_t * list, int index);

int contains_list(Task_List_t * list, TaskCB_t * task);

#endif // TASK_LIST_H

