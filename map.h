#ifndef  MAP_H_
#define  MAP_H_
/**
 * @brief STL map 
 * @file  map.h
 * @author mhw
 * @date 03/11/2014
 * @note rb-tree depend
 */
#include "conf.h"
#include "rb_tree.h"

typedef struct rb_pair{
    s_int32_t first;    /** <key in map*/
    void * second;      /** <value in map*/
}pair_t;

struct rb_map{
    rb_node_t * root;   /**<red black tree as map*/
}rb_map_t;


#endif
