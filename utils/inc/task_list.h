#ifndef TASK_LIST_H
#define TASK_LIST_H

#include "task_cb.h"

typedef struct {
  int num_elements;
  TaskCB_t * root;
  TaskCB_t * tail;
} Task_List_t;

// Creates new Task_List_t on the heap, returns NULL if fails
Task_List_t * new_list();

// Frees list then sets list pointer to NULL
void delete_list(Task_List_t * list);

// Appends a task to the end of the list, returns 0 on success, -1 on fail
int append_to_end_list(Task_List_t * list, TaskCB_t * task);

// Inserts a task into the front of the list, returns 0 on success, -1 on fail
int insert_front_list(Task_List_t * list, TaskCB_t * task);

// Removes task from list, returns 0 on success, -1 on fail
int remove_from_list(Task_List_t * list, TaskCB_t * task);

// Inserts task at index index in the list (zero indexed)
int insert_at_index_list(Task_List_t * list, TaskCB_t * task, int index);

// Checks if a list contains a task: 1 if true, 0 if false
int contains_list(Task_List_t * list, TaskCB_t * task);

// Checks if a list is empty: 1 if empty, 0 if not
int is_empty(Task_List_t * list);

#endif // TASK_LIST_H

