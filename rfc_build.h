#ifndef  RFC_BUILD_H_
#define  RFC_BUILD_H_

/**
 * @brief rfc build
 * @file rfc_build.h
 * @author mhw
 * @note 
 * @date 2014/3/11
 *
 */
#include "rfc_structure.h"
#include "rfc_parse.h"

struct rfc_build{
    struct rfc_mem      *memp;  /** < mem manager ptr*/
    struct agg_master   *agmp;  /** < aggregate map manager ptr, 多核情况下多实体*/
    struct rfc_parse    *parsp; /** < parse ptr*/
    struct slot_table   *slotp; /** < slot table ptr*/
};

struct phase2_element{
    struct ces_table *cetp;     
    struct cbm_table *cbtp;
};

extern struct rfc_build *alloc_buildp(struct rfc_mem *memp, struct agg_master *agmp, struct rfc_parse *parsp, struct slotp *slotp);
void destory_buildp(struct rfc_build *rfc_buildp);


#endif
