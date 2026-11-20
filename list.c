// list/list.c
// 
// Implementation for linked list.
//
// <Author>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "list.h"

list_t *list_alloc() { 
  list_t* mylist =  (list_t *) malloc(sizeof(list_t)); 
  mylist->head = NULL;
  return mylist;  
}

void list_free(list_t *l) {
  node_t *curr = l-> head;
  while (curr != NULL) {
    node_t *next = curr ->next;
    free(curr);
    curr = next;
  }
   free(l);
}

void list_print(list_t *l) {
  node_t *curr = l ->head;
  while (curr != NULL) {
    printf("%d -> ", curr ->value);
    curr = curr ->next;
  }
  printf("NULL\n");  
}

char * listToString(list_t *l) {
  char* buf = (char *) malloc(sizeof(char) * 10024);
  buf[0] = '\0';
  char tbuf[20];

	node_t* curr = l->head;
  while (curr != NULL) {
    sprintf(tbuf, "%d->", curr->value);
    curr = curr->next;
    strcat(buf, tbuf);
  }
  strcat(buf, "NULL");
  return buf;
}

int list_length(list_t *l) {
  int count = 0;
  node_t *curr = l->head;
  while (curr != NULL){
    count++;
    curr = curr->next;
  }
  return count;
}

void list_add_to_back(list_t *l, elem value) {
  node_t *cur_node = getNode(value);

  if (l ->head == NULL) {
    l ->head = cur_node;
    return;
  }
  node_t *curr = l ->head;
  while (curr ->next != NULL){
    curr = curr->next;
  }
  curr->next = cur_node;
}

void list_add_to_front(list_t *l, elem value) {
     node_t *cur_node = (node_t *) getNode(value);

     /* Insert to front */
     node_t *head = l->head;  // get head of list

     cur_node->next = head;
     l->head = cur_node;
}

node_t * getNode(elem value) {
  node_t *mynode;

  mynode = (node_t *) malloc(sizeof(node_t));
  mynode->value = value;
  mynode->next = NULL;

  return mynode;
}


void list_add_at_index(list_t *l, elem value, int index) {
  if (index < 1) return;

  if (index == 1){
    list_add_to_front(l,value);
    return;
  }
    
  node_t *curr = l -> head;
  for (int i=1; i < index -1; i++){
    if (curr == NULL) return;
    curr = curr ->next;
  }
  if (curr == NULL) return;
  node_t *cur_node = getNode(value);
  cur_node -> next = curr -> next;
  curr -> next = cur_node;
    
}

elem list_remove_from_back(list_t *l) { 
  if (l ->head == NULL)return -1;

  if (l ->head->next == NULL){
    elem value = l->head->value;
    free(l->head);
    l->head = NULL;
    return value;
  }
  node_t *curr = l->head;
  while (curr->next->next != NULL) {
    curr = curr->next;
  }
  elem value = curr ->next->value;
  free(curr->next);
  curr->next = NULL;
  return value;
}
elem list_remove_from_front(list_t *l) {
  if (l->head == NULL) return -1;
  node_t *old_head = l->head;
  elem value = old_head -> value;
  l->head = old_head->next;
  free(old_head);
  return value;
}
elem list_remove_at_index(list_t *l, int index) { 
  if (index < 1) return -1;
  if (index == 1) {
    return list_remove_from_front(l);
  }
  node_t *curr = l->head;
  for (int i = 1; i < index - 1; i++) {
    if  (curr == NULL) return -1;
    curr = curr->next;
  }
  if (curr ==NULL || curr ->next == NULL) return -1;
  
  node_t *target = curr->next;
  elem value = target -> value;
  curr ->next = target -> next;
  free(target);
  return value;
}

bool list_is_in(list_t *l, elem value) {
  node_t *curr = l->head;
  while (curr != NULL){
    if (curr->value == value) {
      return true;
    }
    curr = curr->next;
  }
  return false;
}

  elem list_get_elem_at(list_t *l, int index) { 
  if (index <1) return -1;

  node_t *curr = l->head;
  for (int i = 1; i < index; i++){
    if (curr == NULL) return -1;
    curr = curr->next;
  }
  if (curr == NULL) return -1;
  return curr->value;
}
int list_get_index_of(list_t *l, elem value) { 
  node_t *curr = l ->head;
  int index = 1;
  while (curr != NULL) {
    if (curr ->value == value){
        return index;
    }
    curr = curr->next;
    index++;
  }
  return -1;
}