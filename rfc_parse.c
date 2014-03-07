/**
 * @brief rfc parse condition code
 * @file rfc_parse.c
 * @author mhw
 * @date 2014/3/4
 *
 */
#include "rfc_parse.h"


/**
 * @brief init rfc parse 
 * @param[in] memp memory manager
 * @param[in] condition code size size(bytes)
 * return ptr to rfc_parse
 */

struct rfc_parse *init_rfc_parse(struct rfc_mem *memp, u_int32_t size){
    struct rfc_parse * rfc_parse_ret = (struct rfc_parse *)memp->palloc(sizeof(struct rfc_parse));
    if(!rfc_parse_ret)
            return 0;
    rfc_parse_ret->rfc_mem_manager = memp;
    rfc_parse_ret->size = size;
}

/**
 * @brief destory rfc parse
 * @param[in] parsp ptr to rfc parse
 * @return 0;
 *
 */
s_int32_t deinit_rfc_parse(struct rfc_parse * parsp){
    parsp->rfc_mem_manager->pdestory(parsp);
    return 0;
}

/**
 * @brief parse rfc_rule array for row_start to row_end;   
 * @param[in] rfc_rule_param  rfc rule array
 * @param[in] rfc_build_param rfc build manager 
 * @param[in] col index for condition code (16bit/step) 
 * @param[in] row_start pos of the rule index start in rule array
 * @param[in] row_end   pos of the rule index end   in rule array
 * @param[in] arg       other param for callback
 * @param[in] callback  callback(u_int32_t <---index in codition code (16bit/step), u_int16_t <---condition code val in this piece,  
 *                      void * <---- arg 
 * @return 
 * @note spread condition code 
 *
 */
s_int32_t col_parse_rule(struct rfc_rule *rfc_rule_param, struct rfc_build *rfc_build_param, u_int32_t col, u_int32_t row_start, u_int32_t row_end,\
        void *arg1, void *arg2, s_int32_t (*callback)( struct rfc_build *, u_int32_t, u_int16_t, u_int32_t, void arg1*, void *arg2))
{
    u_int16_t i;
    u_int16_t *ptr1, *ptr2;
    u_int32_t eqid;
    while( row_start <= row_end){
        ptr1 = (u_int16_t *)(rfc_rule_param + row_start)->con.param[0];
        ptr2 = (u_int16_t *)(rfc_rule_param + row_start)->con.param[1];
        eqid = (rfc_rule_param + row_start)->eqid;

        ptr1 += col;
        ptr2 += col;
            if(*ptr1 > *ptr2)
                    return -1;
            for(i = *ptr1; i < *ptr2; i++){
                if(callback(rfc_build_param, col, i, eqid, arg1, arg2)){
                        return -1;
                }
            }
        row_start++;
    }
    return 0;
}
























