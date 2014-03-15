#ifndef  RFC_STRUCTURE_H_
#define  RFC_STRUCTURE_H_
/**
 * @brief rfc structure 
 * @file rfc_structure.h
 * @author mhw
 * @note 
 * @date 2014/3/11
 */

#include "rfc_conf.h"
#include "aggregate_master.h"
#include "map.h"

struct rfc_rule{
    void *con[2];
};

struct ces_table{
    u_int16_t cbm_cnt;
    u_int16_t        *table; 
    struct ces_table *next;  
    /*1.in phase0 and phase1  as ptr to cbm_table   
     *2.then convert as ptr to ces table*/
};


struct rfc{
    u_int32_t rule_max;                             /*最大规则数,影响预分配空间*/
    u_int32_t con_len;                              /*特征码长度,字节为单位*/     
    u_int32_t piece_cnt;                            /*分片数*/     
    u_int32_t core_cnt;                             /*核数量*/
    u_int32_t rule_cnt;                             /*实际规则数*/

    s_int32_t *keyval_map;                          /*键值映射表*/
    struct ces_table  **entry;                       /*构建完成后查找入口*/ 
    struct rfc_rule  *rule;                         /*构建初始时rule存放空间*/
    struct agg_master *agmp;                        /*聚合表相关管理 TODO 并行处理开启时需要2选一 
                                                        1.每个构建单元分配一个实体, 高效耗内存
                                                        2.使用读写锁保证安全， 耗时间，内存开销较小*/

    void * (*palloc)(u_int32_t);                    /**<permanent mem alloc*/
    void   (*pdestory)(void *);                     /**<permanent mem free*/

    void * (*talloc)(u_int32_t);                    /**<tmp mem alloc*/
    void   (*tdestory)(void *);                     /**<tmp mem free*/

    s_int32_t status;                               /** <rfc所处阶段 0.init 1.read rule 2.phase0  3.phase1, 4.phase2 5.phase3 6.wait search 7.destory*/
};


struct parse_info{
    u_int32_t pidx;                          /** < 该分片的索引 */
    u_int16_t pval;                          /** < 该分片的值   */
    u_int32_t eqid;                          /** < 该分片的eqid */
    u_int32_t bit_len;                       /** < 该分片所占大小, bit/bit_len, 暂时未使用*/                         
};


enum{
    RFC_INIT,
    RFC_READ,
    RFC_PHASE0,
    RFC_PHASE1,
    RFC_PHASE2,
    RFC_PHASE3,
    RFC_SEARCH,
    RFC_DESTORY
};


#endif

