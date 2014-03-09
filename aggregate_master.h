#ifndef  AGGREGATE_MASTER_H_
#define  AGGREGATE_MASTER_H_
/**
 * @brief aggregatged bitmap manager
 * @file  aggregate_master.h
 * @author xiyan
 * @date 2014/3/09
 */

#include "stdio.h"
#include "string.h"
#include "list.h"

typedef signed int      s_int32_t;
typedef signed short    s_int16_t;
typedef signed char     s_int8_t;

typedef unsigned int    u_int32_t;
typedef unsigned short  u_int16_t;
typedef unsigned char   u_int8_t;


struct agg_bitmap{
    struct      list_head abmp_link;
    u_int32_t   bmp_cnt;                    /** <bitmap tots **/
    /*aggregate bitmap ....*/
    /*bitmap    ...       */
};

#define ABMP_HEAD_SIZE        (sizeof(struct agg_bitmap))

struct agg_table{
    struct      list_head table_link;
    struct      list_head abmp_head;

    u_int32_t   abmp_cnt;

};

#define ABMP_TABLE_SIZE       (sizeof(struct agg_table))

/* agg master*/
struct agg_master{
    struct list_head table_head;
    u_int32_t table_cnt;                      /**<tot table */
    u_int32_t abmp_cnt;                       /**<to aggregated bitmap*/
    u_int32_t mem_cnt;                        /**<tot mem*/              

    void *(*agg_alloc)(u_int32_t);             /**<ptr to alloc mem*/
    void  (*agg_destory)(void *);              /**<ptr to free  mem*/

    u_int32_t agg_bitmap_len;                
    u_int32_t bitmap_len;                   
    u_int32_t bit_len;     

    struct agg_bitmap *mem;                                /**<tmp mem used to aggregated bitmap*/
};

#define ABMP_MASTER_SIZE    (sizeof(struct agg_master))

extern struct agg_master *alloc_agg_master(void *(*agg_alloc)(u_int32_t), void (*agg_destory)(void *), u_int32_t bit_len);
extern void destory_agg_master(struct agg_master *agg_master_par);
extern struct  agg_table * alloc_agg_table(struct agg_master *agg_master_par);
extern void destory_agg_table(struct agg_master *agg_master_par, struct agg_table *agg_table_par);
extern struct agg_bitmap *alloc_agg_bitmap(struct agg_master *agg_master_par);
extern void destory_agg_bitmap(struct agg_master *agg_master_par, struct agg_bitmap * agg_bitmap_par);
extern void add_agg_bitmap(struct agg_table *agg_table_par, struct agg_bitmap *agg_bitmap_par);
extern void add_agg_bitmap_tail(struct agg_table *agg_table_par, struct agg_bitmap *agg_bitmap_par);
extern void remove_agg_bitmap(struct agg_table *agg_table_par, struct agg_bitmap *agg_bitmap_par);
extern struct agg_bitmap *bmp_to_abmp(struct agg_master *agg_master_par, u_int32_t *bmp);
extern u_int32_t *abmp_to_bmp(struct agg_master * agmp, struct agg_bitmap *abmp, u_int32_t *bmp);
extern struct agg_bitmap *abmp_or(struct agg_master *agg_master_par, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern s_int32_t abmp_equal( struct agg_master * agg_master_par, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern s_int32_t abmp_cmp(struct agg_master * agg_master_par, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern u_int32_t search_in_table(struct agg_master *agg_master_par, struct agg_table *agg_table_par, struct agg_bitmap *agg_bitmap_par);
extern void debug_agg_bitmap(struct agg_master *agg_master_par, struct agg_bitmap *agg_bitmap_par);
extern void debug_agg_table(struct agg_master *agg_master_par, struct agg_table *agg_table_par);
extern void debug_agg_master(struct agg_master *agg_master_par);




#endif

