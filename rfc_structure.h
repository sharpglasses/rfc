#ifndef  RFC_STRUCTURE_H_
#define  RFC_STRUCTURE_H_
/**
 * @brief rfc structure 
 * @file rfc_structure.h
 * @author mhw
 * @note 
 * @date 2014/3/3
 */
#include "rfc_mem.h"
#include "rfc_bitmap.h"

struct condition_code{
    void *param[2];
};

struct rfc_rule{
    struct condition_code con;
    s_int32_t             val;
    s_int32_t             eqid;
};

struct ces_table{
    u_int32_t ces_cnt;  
    u_int32_t cbm_cnt;

    u_int32_t        *valid; /*1.in phase0 as  ces_table*/
    struct ces_table *next;  /*1.in phase0 and phase1  as ptr to cbm_table   2.then convert to ptr to ces table*/
};

struct ces_entry{
    u_int32_t entry_cnt;  
    u_int32_t cbm_cnt;

    u_int32_t *key;
    u_int32_t *entry;

    struct ces_table *next;
};


struct cbm_table{
    struct  agg_bitmap *valid;
};

struct slot_table{
    u_int32_t slot_cnt;
    void **valid;
};

#endif

