#ifndef  RFC_MEM_H_
#define  RFC_MEM_H_
/**
 * @brief rfc内存管理
 * @file rfc_mem.h
 * @author mhw
 * @date 2014/3/10
 *
 */
#include "rfc_conf.h"

/*mem manager for rfc*/
struct rfc_mem{
    void * (*talloc)(u_int32_t);                         /**<tmp mem alloc*/
    void   (*tdestory)(void *);                             /**<tmp mem free*/

    void * (*palloc)(u_int32_t);                   /**<permanent mem alloc*/
    void   (*pdestory)(void *);                       /**<permanent mem free*/
};

/**
 * @brief init rfc mem manager
 * @brief talloc tmp memory alloc ptr 
 * @brief tdestory  tmp memory free  ptr 
 * @brief palloc  permanent memory alloc  ptr 
 * @brief pdestory   permanent memory free   ptr 
 * @return rfc_mem
 */
static inline struct rfc_mem * init_rfc_mem(void *talloc(u_int32_t), void tdestory(void *), void *palloc(u_int32_t), void pdestory(void *)){
    struct rfc_mem *mem_ret = (struct rfc_mem*)palloc(sizeof(struct rfc_mem));
    if(!mem_ret)
        return 0;
    memset(mem_ret, 0 , sizeof(struct rfc_mem));
    mem_ret->talloc       = talloc;
    mem_ret->tdestory     = tdestory;
    mem_ret->palloc = palloc;
    mem_ret->pdestory  = pdestory;
    return mem_ret;
}

/**
 * @brief deinit rfc mem manager
 * @param[in] rfc_mem_param rfc_mem wait to destory
 * @return 
 */
static inline s_int32_t deinit_rfc_mem(struct rfc_mem *mem_ptr){
    mem_ptr->pdestory(mem_ptr);
    return 0;
}

#endif
