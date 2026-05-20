#include "list.h"
#include "malloc.h"

List_t * new_list() {
  List_t * list = (List_t *) rtos_malloc(sizeof(List_t));
  list->num_elements = 0;
  list->root = NULL;
  list->tail = NULL;

  return list;
}

int delete_list();

int append_to_end_list(List_t * list, TaskCB_t *task);

int insert_front(List_t * list, TaskCB_t *task);

int delete_from_list(TaskCB_t *task);

int insert_at_index(List_t * list, int index);
