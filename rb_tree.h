#ifndef  RB_TREE_H_
#define  RB_TREE_H_
/**
 * @brief rb_tree copy from july
 * @file rb_tree.h
 * @author july
 * @date 03/11/2014
 * @note july's blog from csdn 
 *
 */

#include "conf.h"

typedef u_int32_t rb_key_t;
typedef u_int32_t rb_data_t;

typedef enum color
{
    RED = 0,
    BLACK = 1
}color_t;


typedef struct rb_node
{
    struct rb_node *left, *right, *parent;
    rb_key_t key;
    rb_data_t data;
    color_t color;
}rb_node_t;

typedef struct rb_tree{
    rb_node_t *root;
    void * (*rb_alloc)(u_int32_t);                     
    void   (*rb_destory)(void *);                          
}rb_tree_t;


extern struct rb_tree * alloc_rbp(void * (*rb_alloc)(u_int32_t), void (*rb_destory)(void *));
extern void destory_rbp(struct rb_tree *rbp);
extern rb_node_t* rb_search(rb_tree_t *rbp, rb_key_t key);
extern rb_node_t* rb_insert(rb_tree_t *rbp, rb_key_t key, rb_data_t data);
extern rb_node_t* rb_erase(rb_tree_t  *rbp, rb_key_t key);


#endif
