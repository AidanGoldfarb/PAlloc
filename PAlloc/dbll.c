#include <stdlib.h>
#include "dbll.h"
#include <stdio.h>

/* Routines to create and manipulate a doubly-linked list */

/* create a doubly-linked list */
/* returns an empty list or NULL if memory allocation failed */
struct dbll *dbll_create()
{
  struct dbll* node = (struct dbll*)malloc(sizeof(struct dbll));
  if(node == NULL){
    return NULL;
  }
  return node;
}

/* frees all memory associated with a doubly-linked list */
/* this must also free all memory associated with the linked list nodes */
/* assumes user data has already been freed */
void dbll_free(struct dbll *list)
{
 // free(list->first);
 // free(list->last);
  free(list);
  
}

/* iterate over items stored in a doubly-linked list */

/* begin iteration from node start, end iteration at node stop (include stop in iteration) */
/* the list is traversed using the next pointer of each node */

/* if start is NULL, then start is the first node */      //DONE 
/* if end is NULL, then end is the last node */           //DONE 
/* at each node, call the user function f with a pointer to the list,
   a pointer to the current node, and the value of ctx */ //DONE            

/* if f returns 0, stop iteration and return 1 */

/* return 0 if you reached the end of the list without encountering end */
/* return 1 on successful iteration */

int dbll_iterate(struct dbll *list,
				 struct llnode *start,
				 struct llnode *end,
				 void *ctx,
				 int (*f)(struct dbll *, struct llnode *, void *))
{
  if(start == NULL){
    start = list->first;
  }
  if(end == NULL){
    end = list->last;
  }

  int flag = 0;
  struct llnode* new_node = start;
                                                                        //while(new_node->next != end && new_node->next != NULL){  //iterate from start to end //end was NULL before
  while(new_node->next != NULL){  //iterate from start to end of list 
    if(f(list, new_node, ctx) == 0) return 1; 
    if(new_node == end){ 
      flag = 1; //'end' encountered 
      return 1;
      }  
    
    new_node = new_node->next;
  }
    if(new_node == list->last){
      if(f(list, new_node, ctx) == 0) return 1; 
      if(new_node == end) return 1;//flag = 1; //'end' encountered 
      
    }
  //
  if(flag == 0) return 0;  //fix: return 0 if reached end of list AND NOT hit 'end'  ('end' isnt even a node)
  
  return 1;
}

/* similar to dbll_iterate, except that the list is traversed using
   the prev pointer of each node (i.e. in the reverse direction).

   Adjust accordingly. For example if start is NULL, then start is the
   last node in the list.  If end is NULL, then end is the first node
   in the list.

*/

int dbll_iterate_reverse(struct dbll *list,
						 struct llnode *start,
						 struct llnode *end,
						 void *ctx,
						 int (*f)(struct dbll *, struct llnode *, void *)
						 )
{
  if(start == NULL){
    start = list->last;
  }
  if(end == NULL){
    end = list->first;
  }

  int flag = 0;
  struct llnode* new_node = start;
                                                                        //while(new_node->next != end && new_node->next != NULL){  //iterate from start to end //end was NULL before
  while(new_node->prev != NULL){  //iterate from start to end of list 
    if(f(list, new_node, ctx) == 0) return 1; 
    if(new_node == end){ 
      flag = 1; //'end' encountered 
      return 1;
      }  
    
    new_node = new_node->prev;
  }
    if(new_node == list->first){
      if(f(list, new_node, ctx) == 0) return 1; 
      if(new_node == end) return 1;//flag = 1; //'end' encountered 
      
    }
  //
  if(flag == 0) return 0;  
  
  return 1;
}


/* Remove `llnode` from `list` */
/* Memory associated with `node` must be freed */
/* You can assume user_data will be freed by somebody else (or has already been freed) */
void dbll_remove(struct dbll *list, struct llnode *node)
{

 if(list->first == node && node->next == NULL){ //if node is the head and only node
   list->first = NULL;
   list->last = NULL;
   free(node);
 }
 else if(list->first == node){
   list->first = node->next;
   list->first->prev = NULL;
   free(node);
 }
 
 else if(list->last == node){
   list->last = node->prev;
   list->last->next = NULL;
   free(node);
 }
 else{
    struct llnode* head = list->first;
    while(head->next != NULL){ //iterate through list
      if(head == node){
        head->prev->next = head->next;
        head->next->prev = head->prev;
        free(node);
        return;
      }
      head = head->next;
    }
 }
 
}

/* Create and return a new node containing `user_data` */
/* The new node must be inserted before `node` */
/* if node is NULL, then insert the new node at the beginning of the list */
/* return NULL if memory could not be allocated */
struct llnode *dbll_insert_before(struct dbll *list, struct llnode *node, void *user_data)
{
  struct llnode* new_node = (struct llnode*)malloc(sizeof(struct llnode));
  if(new_node == NULL){
    return NULL;
  }
  new_node -> user_data = user_data;
  if(list == NULL){
    return NULL;
  } 
  else if(list->first == NULL){ //if list is empty
    new_node-> prev = NULL;
    list->first = new_node;
    list->last = new_node;
    return new_node;
  }
  else if(list->last == NULL ){
    new_node->prev = NULL;
    new_node->next = list->first;
    list->first = new_node;
  }
  else if(node == NULL){  //insert node to start of list
    new_node->prev = NULL;
    new_node->next = list->first;
    list->first->prev = new_node;
    list->first = new_node;
  }
  else{             //insert node to list
    new_node->prev = node->prev;
    new_node->next = node;
    node->prev = new_node;
    if(new_node->prev != NULL){
      new_node->prev->next = new_node;
    }
    // new_node->next = node->next;
    // node->next = new_node;
    // new_node->prev = node;
    // if(new_node->next != NULL){
    //   new_node->next->prev = new_node;
    // }
  }

  return new_node;
}

/* Create and return a new node containing `user_data` */
/* The new node must be inserted after `node` */
/* if node is NULL, then insert the node at the end of the list */

/* return NULL if memory could not be allocated */
struct llnode *dbll_insert_after(struct dbll *list, struct llnode *node, void *user_data)
{
  struct llnode* new_node = (struct llnode*)malloc(sizeof(struct llnode));
  if(new_node == NULL){
    return NULL;
  }
  new_node -> user_data = user_data;
  if(list == NULL){
    return NULL;
  }
  else if(list->first == NULL){ //if list is empty
    new_node-> prev = NULL;
    list->first = new_node;
    list->last = new_node;
    return new_node;
  }
  else if(list->last == NULL ){
    new_node->prev = list->first;
    list->last = new_node;
  }
  else if(node == NULL){  //append node to list
    new_node->next = NULL;
    new_node->prev = list->last;
    list->last->next = new_node;  
    list->last = new_node;
  }
  else{             //insert node to list
    new_node ->next = node->next;
    node->next = new_node;
    new_node->prev = node;
    if(new_node->next != NULL){
      new_node->next->prev = new_node;
    }
  }

  return new_node;
}


/* create and return an `llnode` that stores `user_data` */
/* the `llnode` is added to the end of list */
/* return NULL if memory could not be allocated */
/* this function is a convenience function and can use the dbll_insert_after function */
struct llnode *dbll_append(struct dbll *list, void *user_data)
{
  struct llnode* new_node = (struct llnode*)malloc(sizeof(struct llnode));
  if(new_node == NULL){
    return NULL;
  }
  new_node->user_data = user_data;
  new_node->next = NULL;          //forwards point for new_node

  if(list == NULL || new_node == NULL){  
    return NULL;
  }
  else if(list->first == NULL){ //if list is empty
    new_node-> prev = NULL;
    list->first = new_node;
    list->last = new_node;
    return new_node;
  }
  else if(list->last == NULL){
    new_node->prev = list->first;
    list->last = new_node;
    return new_node;
  }
  else{
    new_node->prev = list->last;      //backwards pointer for new_node
    list->last->next = new_node;      //forwards pointer for node before new_node
    list->last = new_node;
    return new_node;
  }
}

//int main(){}
