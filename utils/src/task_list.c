#include "task_list.h"
#include "malloc.h"
#include "task_cb.h"

Task_List_t * new_list() {
  Task_List_t * list = (Task_List_t *) rtos_malloc(sizeof(Task_List_t));
  if (list == NULL) {
    return NULL;
  }
  list->num_elements = 0;
  list->root = NULL;
  list->tail = NULL;

  return list;
}

void delete_list(Task_List_t * list) {
  rtos_free(list);
  list = NULL;
}

int append_to_end_list(Task_List_t * list, TaskCB_t *task) {
  if (list == NULL || task == NULL) {
    return -1;
  }
  if (list->root == NULL) {
    task->next = NULL;
    task->prev = NULL;
    // enter critical --> port
    list->root = task;
    list->tail = task;
    list->num_elements = 1;
    // exit critical --> port
    return 0;
  }
  else {
    task->next = NULL;
    // enter critical --> port
    task->prev = list->tail;
    list->tail->next = task;
    list->num_elements += 1;
    // exit critical --> port
    return 0;
  }
}

int insert_front_list(Task_List_t * list, TaskCB_t *task) {
  if (list == NULL || task == NULL) {
    return -1;
  }
  if (list->root == NULL) {
    task->next = NULL;
    task->prev = NULL;
    // enter critical
    list->root = task;
    list->tail = task;
    list->num_elements = 1;
    // exit critical
    return 0;
  }
  else {
    task->prev = NULL;
    // enter critical
    task->next = list->root;
    list->root->prev = task;
    list->root = task;
    list->num_elements += 1;
    // exit critical
    return 0;
  }
}

int remove_from_list(Task_List_t * list, TaskCB_t *task) {
  if (task == NULL) {
    return -1;
  }
  if (task == list->tail && task == list->root) {
    // enter critical
    list->tail = NULL;
    list->root = NULL;
    list->num_elements = 0;
    // exit critical
    task->next = NULL;
    task->prev = NULL;
    return 0;
  }
  else if (task == list->tail) {
    // enter critical
    list->tail = task->prev;
    list->tail->next = NULL;
    list->num_elements -= 1; 
    // exit critical
    task->next = NULL;
    task->prev = NULL;
    return 0;
  }
  else if (task == list->root) {
    // enter critical
    list->root = task->next;
    list->root->prev = NULL;
    list->num_elements -= 1;
    // exit critical
    task->next = NULL;
    task->prev = NULL;
    return 0;
  }
  else {
    // enter critical
    task->next->prev = task->prev;
    task->prev->next = task->next;
    list->num_elements -= 1;
    // exit critical
    
    task->next = NULL;
    task->prev = NULL;
    list = NULL;
    return 0;
  }
}

int insert_at_index_list(Task_List_t * list, int index);

int contains_list(Task_List_t * list, TaskCB_t * task);
