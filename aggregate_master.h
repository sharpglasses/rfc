#ifndef  AGGREGATE_MASTER_H_
#define  AGGREGATE_MASTER_H_
/**
 * @brief aggregatged bitmap manager
 * @file  aggregate_master.h
 * @author mhw
 * @date 2014/3/11
 */

#include "conf.h"
#include "list.h"

struct agg_bitmap{
    struct      list_head abmp_link;
    u_int32_t   bmp_cnt;                    /** <bitmap tots      */
                                            /** <aggregate bitmap */
                                            /** <bitmap           */
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
    u_int32_t table_cnt;                    /** <tot table            */
    u_int32_t abmp_cnt;                     /** <to aggregated bitmap */
    u_int32_t mem_cnt;                      /** <tot mem              */              

    void *(*agg_alloc)(u_int32_t);          /** <ptr to alloc mem     */
    void  (*agg_destory)(void *);           /** <ptr to free  mem     */

    u_int32_t agg_bitmap_len;               /** < aggregated bmp len(32bit/step) */ 
    u_int32_t bitmap_len;                   /** < normal bitmap  len(32bit/step) */
    u_int32_t bit_len;                      /** < bitmap len        (1bit/step)  */

    struct agg_bitmap *mem;                 /** < tmp mem used for aggregated bitmap*/
};

#define ABMP_MASTER_SIZE    (sizeof(struct agg_master))

extern struct agg_master *alloc_agmp(void *(*agg_alloc)(u_int32_t), void (*agg_destory)(void *), u_int32_t bit_len);
extern void destory_agmp(struct agg_master *agmp);
extern struct  agg_table * alloc_agtp(struct agg_master *agmp);
extern void destory_agtp(struct agg_master *agmp, struct agg_table *agtp);
extern struct agg_bitmap *alloc_abmp(struct agg_master *agmp);
extern void destory_abmp(struct agg_master *agmp, struct agg_bitmap * abmp);
extern void add_abmp(struct agg_table *agtp, struct agg_bitmap *abmp);
extern void add_abmp_tail(struct agg_table *agtp, struct agg_bitmap *abmp);
extern void remove_abmp(struct agg_table *agtp, struct agg_bitmap *abmp);
extern struct agg_bitmap *bmp_to_abmp(struct agg_master *agmp, u_int32_t *bmp);
extern u_int32_t *abmp_to_bmp(struct agg_master * agmp, struct agg_bitmap *abmp, u_int32_t *bmp);
extern struct agg_bitmap *abmp_and(struct agg_master *agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern struct agg_bitmap *abmp_or(struct agg_master *agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern s_int32_t abmp_equal( struct agg_master * agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern s_int32_t abmp_cmp(struct agg_master * agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2);
extern u_int32_t search_abmp_in_table(struct agg_master *agmp, struct agg_table *agtp, struct agg_bitmap *abmp);
extern void debug_abmp(struct agg_master *agmp, struct agg_bitmap *abmp);
void insert_sort_agtp(struct agg_master *agmp, struct agg_table *agtp);
extern void debug_agtp(struct agg_master *agmp, struct agg_table *agtp);
extern void debug_agmp(struct agg_master *agmp);

#endif

