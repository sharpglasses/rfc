#ifndef  RB_MAP_H_
#define  RB_MAP_H_
/**
 * @brief rb_map copy from july
 * @file rb_map.h
 * @author july
 * @date 03/11/2014
 * @note july's blog from csdn 
 * @note change from july's blog from csdn 
 *      1.mem cost much
 *      2.good at search
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

typedef struct rb_map{
    rb_node_t *root;
    void * (*rb_alloc)(u_int32_t);                     
    void   (*rb_destory)(void *);                          
}rb_map_t;

extern struct rb_map * alloc_rbp(void * (*rb_alloc)(u_int32_t), void (*rb_destory)(void *));
extern void            destory_rbp(struct rb_map *rbp);
extern rb_node_t*      rbp_search(rb_map_t *rbp, rb_key_t key);
extern s_int32_t       rbp_set(rb_map_t *rbp, rb_key_t key, rb_data_t data);
extern void            rbp_erase(rb_map_t  *rbp, rb_key_t key);


#endif
