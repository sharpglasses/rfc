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

struct rb_map{
    rb_node_t * root;   /**<red black tree as map*/
    void * (*rb_alloc)(u_int32_t);                     
    void   (*rb_destory)(void *);                          
}rb_map_t;



#endif
