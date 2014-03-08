#ifndef  RFC_MEM_H_
#define  RFC_MEM_H_
/**
 * @brief rfc内存管理
 * @file rfc_mem.h
 * @author mhw
 * @date 2014/3/11
 *
 */
#include "rfc_conf.h"

/*mem manager for rfc*/
struct rfc_mem{
};


struct rfc_mem * alloc_rfc_mem(void *talloc(u_int32_t), void tdestory(void *), void *palloc(u_int32_t), void pdestory(void *));
void destory_rfc_mem(struct rfc_mem *mem_ptr);

#endif
