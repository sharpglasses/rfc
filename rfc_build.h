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
    struct rfc_mem      *memp;
    struct agg_master   *agmp;
    struct rfc_parse    *parsp;
    struct slot_table   *slotp;
};

struct phase2_element{
    struct ces_table *ces_table_ptr;
    struct cbm_table *cbm_table_ptr;
};


extern struct rfc_build *init_rfc_build(struct rfc_mem *rfc_mem_param, struct rfc_bitmap *rfc_bitmap_param, rfc_parse *rfc_parse_param);
extern s_int32_t deinit_rfc_build(struct rfc_build *rfc_build_param);


#endif
