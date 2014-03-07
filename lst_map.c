#include "lst_map.h"

/**
 * @brief LST_MAP
 * @file  lst_map.c
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


static void add_lstnp(struct lst_map *lstmp, struct lst_node *lstnp);
static void remove_lstnp(struct lst_map *lstmp, struct lst_node *lstnp);
static struct  lst_node * alloc_lstnp(struct lst_map *lstmp);
static void destory_lstnp(struct lst_map *lstmp, struct lst_node *lstnp);




/**
 * @brief del pair in map
 * @param[in] lstmp map
 * @param[in] lstnp pair 
 * @return 
 */
void  del_lstmp_node(struct lst_map *lstmp, struct lst_node* lstnp)
{
    remove_lstnp(lstmp, lstnp);
    destory_lstnp(lstmp, lstnp);
}

/**
 * @brief find pair in map
 * @param[in] lstmp map
 * @param[in] key 
 * @return 
 */
struct lst_node *search_in_lstmp(struct lst_map *lstmp, u_int32_t key)
{
        struct lst_node *lstnp;
        struct list_head  *pos;
        list_for_each(pos, &lstmp->lst_head){
            lstnp = container_of(pos, struct lst_node, lst_link);
            if(lstnp->key == key){
                    return lstnp;
            }
       }
       return 0;
}

/**
 * @brief map[key] = val
 * @param[in] lstmp map
 * @param[in] key 
 * @return -1 alloc space fail
 * @return  0  success
 */
s_int32_t set_lstmp_val(struct lst_map *lstmp, u_int32_t key, u_int32_t val)
{
    struct lst_node *lstnp = search_in_lstmp(lstmp, key);
    if(!lstnp){
        lstnp = alloc_lstnp(lstmp);
        add_lstnp(lstmp, lstnp);
    }
    if(!lstnp){
        return -1;
    }
    lstnp->key = key;
    lstnp->val = val;
    return 0;
}



/**
 * @brief create map
 * @param[in] lst_alloc   ptr for alloc mem
 * @param[in] lst_destory ptr for destory mem
 * @return 
 *
 */
struct lst_map * alloc_lstmp(void * (*lst_alloc)(u_int32_t), void (*lst_destory)(void *))
{
        struct lst_map *lstmp = (struct lst_map *)lst_alloc(sizeof(struct lst_map));
        if(!lstmp){
            return 0;
        }
        memset(lstmp,  0,  sizeof(struct lst_map));
        INIT_LIST_HEAD(&lstmp->lst_head);
        lstmp->lst_alloc       = lst_alloc;
        lstmp->lst_destory     = lst_destory;
        lstmp->lst_cnt         = 0;
        return lstmp;
}

/**
 * @brief destory map
 * @return 
 *
 */
void destory_lstmp(struct lst_map *lstmp)
{
    struct lst_node *lstnp;
    while(lstmp->lst_cnt){
            lstnp = container_of(lstmp->lst_head.next, struct lst_node, lst_link);
            del_lstmp_node(lstmp, lstnp);
    }
    lstmp->lst_destory(lstmp);
}

/**
 * @brief  alloc lst_node 
 * @param[in] lstnp  ptr to the lst_map 
 * @return  ptr to new lst_node 
 */
static struct  lst_node * alloc_lstnp(struct lst_map *lstmp){
    struct lst_node *lstnp  = (struct lst_node *)lstmp->lst_alloc( sizeof(struct lst_node));
    if(!lstnp){
        return 0;
    }
    memset(lstnp, 0, sizeof(struct lst_node));
    INIT_LIST_HEAD(&lstnp->lst_link);
    return lstnp;
};

/**
 * @brief destory_lstnp
 * @param[in] lstmp ptr to the lst_map
 * @param[in] lstmp ptr to the lst_np
 * @return 
 */
static void destory_lstnp(struct lst_map *lstmp, struct lst_node *lstnp)
{
    lstmp->lst_destory(lstnp);
}

/**
 * @brief add  lst_node into lst_map tail
 * @param[in]  lstmp  the table want to save this aggregated bitmap
 * @param[in]  lstnp the ptr to the aggregated bitmap
 */
static void add_lstnp(struct lst_map *lstmp, struct lst_node *lstnp){
    list_add_tail(&lstnp->lst_link, &lstmp->lst_head);
    lstmp->lst_cnt++;
}

/**
 * @brief remove lst_node from the lst_map 
 * @param[in]  lstmp  the aggregated bitmap in 
 * @param[in]  lstnp the ptr to the aggregated bitmap
 */
static void remove_lstnp(struct lst_map *lstmp, struct lst_node *lstnp){
    list_del(&lstnp->lst_link);
    lstmp->lst_cnt--;
}


/**
 * @brief debug lst_node
 * @param[in] lstnp ptr to the lst_node
 */
void debug_lstnp(struct lst_node *lstnp){
    fprintf(stderr, "lst_node:\n");
    fprintf(stderr, "key %u\n", lstnp->key);          
    fprintf(stderr, "val %u\n", lstnp->val);          
}


/**
 * @brief debug lst_map
 * @param[in] lstmp ptr to the lst_map
 */
void debug_lstmp(struct lst_map *lstmp){
        struct lst_node *lstnp;
        struct list_head  *pos;
        u_int32_t i = 0;
        fprintf(stderr, "lst_map:\n");
        fprintf(stderr, "lst_map->lst_cnt: %u\n", lstmp->lst_cnt);
        fprintf(stderr, "map items:\n");
        list_for_each(pos, &lstmp->lst_head){
            fprintf(stderr, "item %u\n", i++);
            lstnp = container_of(pos, struct lst_node, lst_link);
            debug_lstnp(lstnp);
        }
        fprintf(stderr, "\n");
}

s_int32_t traversal_in_lstmp(struct lst_map *lstmp, void *input, void *output, s_int32_t (*callback)(u_int32_t i, struct lst_map *, struct lst_node*, void *, void *)){
        struct lst_node *lstnp;
        struct list_head  *pos;
        u_int32_t i = 0;
        list_for_each(pos, &lstmp->lst_head){
            lstnp = container_of(pos, struct lst_node, lst_link);
            if(callback(i, lstmp, lstnp, input, output) < 0){
                return -1;
            }
            i++;
        }
        return 0;
}


/**
 * @brief code for debug below
 *
 *
 */



/*
s_int32_t lst_callback(u_int32_t i, struct lst_map *lstmp, struct lst_node *lstnp, void *input, void *output){
    //debug_lstmp(lstmp);
    fprintf(stderr, "get idx %u from call\n", i);
    debug_lstnp(lstnp);
    return 0;
}


int main()
{
    struct lst_map *lstmp = alloc_lstmp(malloc, free);
    struct lst_node *lstnp;
    fprintf(stderr, "set ret %d\n", set_lstmp_val(lstmp, 0, 1));
    fprintf(stderr, "set ret %d\n", set_lstmp_val(lstmp, 1, 1));
    fprintf(stderr, "set ret %d\n", set_lstmp_val(lstmp, 2, 2));
    fprintf(stderr, "set ret %d\n", set_lstmp_val(lstmp, 1, 0));
    lstnp = search_in_lstmp(lstmp, 6);
    if(!lstnp){
        fprintf(stderr, "not found\n");
    }
    lstnp = search_in_lstmp(lstmp, 1);
    if(!lstnp){
        fprintf(stderr, "not found\n");
    }
    debug_lstnp(lstnp);

    debug_lstmp(lstmp);
    del_lstmp_node(lstmp, lstnp);
    debug_lstmp(lstmp);
    traversal_in_lstmp(lstmp, NULL, NULL, lst_callback);
    destory_lstmp(lstmp);
return 0;
}

*/
