#ifndef  RFC_PARSE_H_
#define  RFC_PARSE_H_
/**
 * @brief rfc parse condition code
 * @file rfc_parse.h
 * @author mhw
 * @date 2014/3/3
 *
 */
#include "rfc_mem.h"
#include "rfc_structure.h"


struct rfc_parse{
    struct rfc_mem *rfc_mem_manager;
    u_int32_t size;
};


extern struct rfc_parse *init_rfc_parse(struct rfc_mem *rfc_mem_param, u_int32_t size);
extern s_int32_t deinit_rfc_parse(struct rfc_parse * rfc_parse_param);
extern s_int32_t col_parse_rule(struct rfc_rule *rfc_rule_param, struct rfc_build *rfc_build_param, u_int32_t col, u_int32_t row_start, u_int32_t row_end,\
        void *arg1, void *arg2, s_int32_t (*callback)( struct rfc_build *, u_int32_t, u_int16_t, void arg1*, void *arg2))
#endif
