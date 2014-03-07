#ifndef  RB_MAP_H_
#define  RB_MAP_H_
/**
 * @brief STL map 
 * @file  rb_map.h
 * @author mhw
 * @date 03/11/2014
 * @note 
 *  0.rb-tree depend
 *  1.quick search 
 *  2.mem cost too much
 *  3.intersval cost too much
 *
 */
#include "conf.h"
#include "rb_tree.h"

typedef struct rb_map{
    rb_node_t * root;   /**<red black tree as map*/
    void * (*rb_alloc)(u_int32_t);                     
    void   (*rb_destory)(void *);                          
}rb_map_t;


extern rb_node_t* rb_search(rb_key_t key, rb_node_t* root);
extern rb_node_t* rb_insert(rb_key_t key, rb_data_t data, rb_node_t* root);
extern rb_node_t* rb_erase(rb_key_t key, rb_node_t *root);


extern struct rb_map * init_rbmp(void * (*rb_alloc)(u_int32_t), void (*rb_destory)(void *));
extern void destory_rbmp(struct rb_map *rbmp);
extern struct rb_node_t *search_in_rbmp(struct rb_map *rbmp, u_int32_t key);
extern s_int32_t set_rbmp(struct rb_map *rbmp, u_int32_t key, u_int32_t val);
void   rbmp_erase(struct rb_map *rbmp, u_int32_t key);


#endif
