#include "dbll.h"
#include <stdlib.h>
#include "poolalloc.h"
#include <stdio.h>
/*
   a pool-based allocator that uses doubly-linked lists to track
   allocated and free blocks
 */

/* create and initialize a memory pool of the required size */
/* use malloc() or calloc() to obtain this initial pool of memory from the system */
struct memory_pool *mpool_create(size_t size)
{
  /* set start to memory obtained from malloc */
  /* set size to size */
  struct memory_pool* mpool = (struct memory_pool*)malloc(size);
  if(mpool == NULL){
    return NULL;
  }
  mpool->size = size;
  char* c = (char*)mpool;
  mpool->start = c;
  /* create a doubly-linked list to track allocations */
  /* create a doubly-linked list to track free blocks */
  struct dbll* alloc_list = dbll_create();  //creates lists
  struct dbll* free_list = dbll_create();
  mpool->alloc_list = alloc_list;           //adds lists to mpool
  mpool->free_list = free_list;

  /* create a free block of memory for the entire pool and place it on the free_list */
  struct alloc_info* alloc_info = (struct alloc_info*)malloc(sizeof(struct alloc_info));
  if(alloc_info == NULL){
    return NULL;
  }
  alloc_info->offset = 0;
  alloc_info->size = size;

  struct llnode* free_list_node = (struct llnode*)malloc(sizeof(struct llnode));
  if(free_list_node == NULL){
    return NULL;
  }
  free_list_node->user_data = alloc_info;
  free_list->first = free_list_node;
  free_list->last = free_list_node;
  /* return memory pool object */
  return mpool;
}

/* ``destroy'' the memory pool by freeing it and all associated data structures */
/* this includes the alloc_list and the free_list as well */
void mpool_destroy(struct memory_pool *p)
{
  /* make sure the allocated list is empty (i.e. everything has been freed) */
  /* free the alloc_list dbll */
  /* free the free_list dbll  */
  /* free the memory pool structure */
  free(p->alloc_list);
  free(p->free_list);
  free(p);
}

struct alloc_info* createAlloc_info(size_t size, size_t offset, size_t request_size){
    struct alloc_info* alloc_info = (struct alloc_info*)malloc(sizeof(struct alloc_info));
    alloc_info -> size = size;
    alloc_info ->offset = offset;
    alloc_info -> request_size  = request_size;
    return alloc_info;
}
size_t getBlockSize(size_t size){
  int ofst = 0;     //offset
  if(size == 1) ofst  = 1;
  else if(size == 2) ofst = 2;
  else if(size == 3 || size == 4) ofst = 4;
  else if(size>=5 && size<=8) ofst = 8;
  else ofst = 16;
  return ofst;
}

size_t getAllignedSize(size_t size){
  int ofst = 0;     //offset
  if(size == 1) ofst  = 1;
  else if(size == 2) ofst = 2;
  else if(size == 3 || size == 4) ofst = 4;
  else if(size>=5 && size<=8) ofst = 8;
  else ofst = size+(16 - size%16);
  return ofst;
}

/* allocate a chunk of memory out of the free pool */

/* Return NULL if there is not enough memory in the free pool */

/* The address you return must be aligned to 1 (for size=1), 2 (for
   size=2), 4 (for size=3,4), 8 (for size=5,6,7,8). For all other
   sizes, align to 16.
*/
                                 
void *mpool_alloc(struct memory_pool *p, size_t size)
{
  int allignedSize = getAllignedSize(size);     //offset
  

  
  void *region = NULL;
  /* check if there is enough memory for allocation of `size` (taking
	 alignment into account) by checking the list of free blocks */ 
   /* search the free list for a suitable block */  
  struct llnode* head = p->free_list->first;
  struct alloc_info* alloc_info_pointer = (struct alloc_info*)head->user_data;
  int start = 0;

  if(head->next == NULL){
    if(alloc_info_pointer->size == allignedSize){
        printf("here1\n");
        region = &(head->user_data); //address of info block        
        dbll_append(p->alloc_list,region);//add to alloc list
        dbll_remove(p->free_list, region);//remove from free list
        return p->start + alloc_info_pointer->offset;
    }
    else if(alloc_info_pointer->size > allignedSize || size>= 512){
        //printf("here2\n");
        
          region = &(head->user_data); //address of info block//get the infoBlock (user data)
          size_t k = getBlockSize(size) + (alloc_info_pointer->offset % 4);
          struct alloc_info* alloc_info1 = createAlloc_info(allignedSize, k + alloc_info_pointer->offset, size);//chop the infoBlock into correctly aligned parts
          struct alloc_info* alloc_info2 = createAlloc_info(k, alloc_info_pointer->offset, alloc_info_pointer->size - allignedSize);//chop the infoBlock into correctly aligned parts
          struct alloc_info* alloc_info3 = createAlloc_info(alloc_info_pointer->size - k - allignedSize, alloc_info_pointer->offset + k + allignedSize, alloc_info_pointer->size - k - allignedSize);
          dbll_append(p->alloc_list, alloc_info1);
          dbll_insert_before(p->free_list, head, alloc_info2);
          dbll_insert_after(p->free_list, head, alloc_info3);
          dbll_remove(p->free_list, region);
          return p->start + alloc_info1->offset;
    }
    else{
      return NULL;
    }
    //mpool_free(p,region);
  }

  else{
    while(head->next != NULL){    //traverses free_list  
      start++; 
      if(alloc_info_pointer->size == allignedSize){
        printf("here3\n");
        region = &(head->user_data); //address of info block        
        dbll_append(p->alloc_list,region);//add to alloc list
        dbll_remove(p->free_list, region);//remove from free list
        return p->start + alloc_info_pointer->offset;
    }
    else if(alloc_info_pointer->size > allignedSize){
        printf("here4\n");
        region = &(head->user_data); //address of info block//get the infoBlock (user data)
        size_t k = getBlockSize(size) + (alloc_info_pointer->offset % 4);
        struct alloc_info* alloc_info1 = createAlloc_info(allignedSize, k + alloc_info_pointer->offset , size);//chop the infoBlock into correctly aligned parts
        struct alloc_info* alloc_info2 = createAlloc_info(k, alloc_info_pointer->offset, alloc_info_pointer->size - allignedSize);//chop the infoBlock into correctly aligned parts
        struct alloc_info* alloc_info3 = createAlloc_info(alloc_info_pointer->size - k - allignedSize, alloc_info_pointer->offset + k + allignedSize, alloc_info_pointer->size - k - allignedSize);
        dbll_append(p->alloc_list, alloc_info1);
        dbll_insert_before(p->free_list, head, alloc_info2);
        dbll_insert_after(p->free_list, head, alloc_info3);
        dbll_remove(p->free_list, region);
        return p->start + alloc_info1->offset;
    }
    else
      return NULL;
    }
    return NULL;
  }

  
  /* there are many strategies you can use: first fit (the first block that fits),
	 best fit (the smallest block that fits), etc. */

  /* if no suitable block can be found, return NULL */
  // if(found == 0){
  //   return NULL;
  // }
  // /* if found, create an alloc_info node, store start of new region
	//  into offset, set size to allocation size (take alignment into
	//  account!), set free to null */
  // struct alloc_info* alloc_info_node = (struct alloc_info*)malloc(sizeof(struct alloc_info));
  // if(alloc_info_node == NULL){
  //   return NULL;
  // }
  // alloc_info_node->offset = start;
  // alloc_info_node->size = size+start; //size + offset?
  // alloc_info_node->request_size = size;
  
  // /* add the new alloc_info node to the memory pool's allocated
	//  list */
  // struct llnode* new_node = (struct llnode*)malloc(sizeof(struct llnode));
  // if(new_node == NULL){
  //   return NULL;
  // }
  // new_node->user_data = alloc_info_node;
  // dbll_append(p->alloc_list, new_node);
  // /* return pointer to allocated region*/
  // return region;

}

struct llnode* coalesce(struct llnode* left, struct llnode* right){
    struct llnode* new_node = (struct llnode*)malloc(sizeof(struct llnode));
    struct alloc_info* alloc_info_node = (struct alloc_info*)malloc(sizeof(struct alloc_info));

    if(new_node == NULL || alloc_info_node == NULL){
      return NULL;
    }
    struct alloc_info* alloc_info_left = (struct alloc_info*)left->user_data;
    struct alloc_info* alloc_info_right = (struct alloc_info*)right->user_data;
    size_t size = alloc_info_left->size + alloc_info_right->size;


    alloc_info_node->offset = alloc_info_left->offset;
    alloc_info_node->size = size;
    alloc_info_node->request_size = size;

    new_node->user_data = alloc_info_node;

    return new_node;
}

/* Free a chunk of memory out of the pool */
/* This moves the chunk of memory to the free list. */
/* You may want to coalesce free blocks [i.e. combine two free blocks
   that are are next to each other in the pool into one larger free
   block. Note this requires that you keep the list of free blocks in order */
void mpool_free(struct memory_pool *p, void *addr)
{

  /* search the alloc_list for the block */
  struct llnode* head = p->alloc_list->first;
  while(head->next != NULL){
    if(head == addr){
      dbll_append(p->free_list, addr);//add to free_list
      dbll_remove(p->alloc_list, addr);//remove from alloc_list
    }
    head = head->next;
  }
  /* move it to the free_list */
  /* coalesce the free_list */ 
 
  struct llnode* head2 = p->free_list->first;
  if(head2->next != NULL){
      head2 = head2->next;
      while(head2->next != NULL){
        if(head2->prev->user_data == NULL && head2->next->user_data == NULL){//check left and right of address
          struct llnode* big_node = coalesce(head2->prev, head2);
          struct llnode* big_node2 = coalesce(head2->next, head2);
          struct llnode* big_node3 = coalesce(big_node, big_node2);
          dbll_insert_before(p->free_list, head2, &big_node3);//list,node,data
          dbll_remove(p->free_list, head2->prev); //list,node
          dbll_remove(p->free_list, head2); //list,node
          dbll_remove(p->free_list, head2->next); //list,node
       }else if(head2->prev->user_data == NULL){
          struct llnode* big_node = coalesce(head2->prev, head2);
          dbll_insert_before(p->free_list, head, &big_node);//list,node,data
          dbll_remove(p->free_list, head2->prev); //list,node
          dbll_remove(p->free_list, head2); //list,node
       }else if(head2->next->user_data == NULL){
          struct llnode* big_node = coalesce(head2->next, head2);
          dbll_insert_before(p->free_list, head, &big_node);//list,node,data
          dbll_remove(p->free_list, head2->next); //list,node
          dbll_remove(p->free_list, head2); //list,node
       }
    }
  }
  
}


