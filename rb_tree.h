#ifndef  RB_TREE_H_
#define RB_TREE_H_
/**
 * @brief rb_tree copy from july
 * @file rb_tree.h
 * @author july
 * @date
 * @note july's blog from csdn 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef int s_int32_t ;


typedef s_int32_t key_t;
typedef void *data_t;

typedef enum color
{
    RED = 0,
    BLACK = 1
}color_t;


typedef struct rb_node
{
    struct rb_node *left, *right, *parent;
    key_t key;
    data_t data;
    color_t color;
}rb_node_t;


extern rb_node_t* rb_search(key_t key, rb_node_t* root);
extern rb_node_t* rb_insert(key_t key, data_t data, rb_node_t* root);
extern rb_node_t* rb_erase(key_t key, rb_node_t *root);


#endif
