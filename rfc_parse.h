#ifndef  RFC_PARSE_H_
#define  RFC_PARSE_H_
/**
 * @brief rfc parse condition code
 * @file rfc_parse.h
 * @author mhw
 * @date 2014/3/3
 *
 */
#include "rfc_conf.h"
#include "rfc_mem.h"
#include "rfc_structure.h"

struct rfc_parse{
    void *(*agg_alloc)(u_int32_t);          /** <ptr to alloc mem     */
    void  (*agg_destory)(void *);           /** <ptr to free  mem     */
    u_int32_t size;
};

struct parse_coin{
    u_int32_t key;                          /** < piece idx in condition code */
    u_int32_t val;                          /** < val of this piece*/
    u_int32_t eqid;                         /** < eqid of this rule*/
};


extern struct rfc_parse *alloc_rfc_parse(struct rfc_mem *rfc_mem_param, u_int32_t size);
extern s_int32_t destory_rfc_parse(struct rfc_parse * parsp);
extern s_int32_t col_parse_rule(struct rfc_rule *rfc_rule_param, struct rfc_build *rfc_build_param, u_int32_t col, u_int32_t row_start, u_int32_t row_end,\
        void *arg1, void *arg2, s_int32_t (*callback)( struct rfc_build *, u_int32_t, u_int16_t, void arg1*, void *arg2))
#endif
