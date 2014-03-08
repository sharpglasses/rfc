#ifndef  RFC_STRUCTURE_H_
#define  RFC_STRUCTURE_H_
/**
 * @brief rfc structure 
 * @file rfc_structure.h
 * @author mhw
 * @note 
 * @date 2014/3/11
 */

#include "aggregate_map.h"
#include "map.h"

struct condition_code{
    void *param[2];
};

struct rfc_rule{
    struct condition_code con;
    u_int32_t             eqid;     
};

struct ces_table{
    u_int32_t ces_cnt;  
    u_int32_t cbm_cnt;

    u_int32_t        *table; 
    struct ces_table *next;  /*1.in phase0 and phase1  as ptr to cbm_table   2.then convert as ptr to ces table*/
};

struct ces_entry{
    u_int32_t ces_cnt;  
    u_int32_t cbm_cnt;
    struct lst_map   *entry;
    struct ces_table *next;
};


//rb_map as key-val map

//agg master work as cbm table

struct slot_table{
    u_int32_t slot_cnt;
    void **slots;
};

#endif

