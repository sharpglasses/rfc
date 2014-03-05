#ifndef  RFC_MEM_H_
#define  RFC_MEM_H_
/**
 * @brief rfc内存管理
 * @file rfc_mem.h
 * @author mhw
 * @date 2014/3/3
 *
 */
#include "rfc_conf.h"


/*mem manager for rfc*/
struct rfc_mem{
    void * (*tmp_alloc)(u_int32_t);                         /**<tmp mem alloc*/
    void   (*tmp_free)(void *);                             /**<tmp mem free*/

    void * (*permanent_alloc)(u_int32_t);                   /**<permanent mem alloc*/
    void   (*permanent_free)(void *);                       /**<permanent mem free*/
};


/**
 * @brief init rfc mem manager
 * @brief tmp_alloc tmp memory alloc ptr 
 * @brief tmp_free  tmp memory free  ptr 
 * @brief permanent_alloc  permanent memory alloc  ptr 
 * @brief permanent_free   permanent memory free   ptr 
 * @return rfc_mem
 */
static inline struct rfc_mem * init_rfc_mem(void *tmp_alloc(u_int32_t), void tmp_free(void *), \
        void *permanent_alloc(u_int32_t), void permanent_free(void *)){
    struct rfc_mem *rfc_mem_ret = (struct rfc_mem*)permanent_alloc(sizeof(struct rfc_mem));
    if(!rfc_mem_ret)
        return 0;
    memset(rfc_mem_ret, 0 , sizeof(struct rfc_mem));
    rfc_mem_ret->tmp_alloc       = tmp_alloc;
    rfc_mem_ret->tmp_free        = tmp_free;
    rfc_mem_ret->permanent_alloc = permanent_alloc;
    rfc_mem_ret->permanent_free  = permanent_free;
    return rfc_mem_ret;
}


/**
 * @brief deinit rfc mem manager
 * @param[in] rfc_mem_param rfc_mem wait to destory
 * @return 
 */
static inline s_int32_t deinit_rfc_mem(struct rfc_mem *rfc_mem_param){
    rfc_mem_param->permanent_free(rfc_mem_param);
    return 0;
}



#endif
