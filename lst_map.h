#ifndef  LST_MAP_H_
#define  LST_MAP_H_

/**
 * @brief LST_MAP
 * @file  lst_map.h
 * @author mhw
 * @date 03/11/2014
 * @note 
 *      0.lst.h depend
 *      1.mem cost smaller than rb-tree
 *      2.good at buiding
 *      3.be well  in intersaval 
 *      4.bad in search
 *
 */
#include "conf.h"
#include "list.h"

struct lst_map{
    struct list_head lst_head;   
    u_int32_t        lst_cnt; 
    void * (*lst_alloc)(u_int32_t);                     
    void   (*lst_destory)(void *);                          
};

struct lst_node{
    struct list_head lst_link;
    u_int32_t key;            /** <key in map*/
    u_int32_t val;           /** <value in map*/
};


extern struct lst_map * init_lstmp(void * (*lst_alloc)(u_int32_t), void (*lst_destory)(void *));
extern void destory_lstmp(struct lst_map *lstmp);
extern struct lst_node *search_in_lstmp(struct lst_map *lstmp, u_int32_t key);
extern s_int32_t set_lstmp_val(struct lst_map *lstmp, u_int32_t key, u_int32_t val);
void  del_lstmp_node(struct lst_map *lstmp, struct lst_node* lstnp);
extern s_int32_t traversal_in_lstmp(struct lst_map *lstmp, void *input, void *output, s_int32_t (*callback)(u_int32_t i, struct lst_map *, struct lst_node*, void *, void *));
extern void debug_lstnp(struct lst_node *lstnp);
extern void debug_lstmp(struct lst_map *lstmp);


#endif
