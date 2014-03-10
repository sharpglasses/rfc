#include "rfc_debug.h"
/**
 * @brief 存放rfc调试接口
 * @file rfc_debug.h
 * @author mhw
 * @date 2014/3/12
 *
 *
 *
 */



/**
 * @brief 打印读取rule阶段以及初始阶段的rfc信息
 * @param[in] rfcp ptr to rfc
 *
 *
 */
void debug_rfc_read( struct rfc *rfcp){
    u_int32_t i;
    rb_node_t *node;
    fprintf(stderr, "rfc :\n");
    fprintf(stderr, "rfc");
    fprintf(stderr, "rfc->rule_max  :%u\n", rfcp->rule_max);                             /*最大规则数,影响预分配空间*/
    fprintf(stderr, "rfc->con_len   :%u\n", rfcp->con_len);                              /*特征码长度,字节为单位*/     
    fprintf(stderr, "rfc->piece_cnt :%u\n", rfcp->piece_cnt);                            /*分片数*/     
    fprintf(stderr, "rfc->core_cnt  :%u\n", rfcp->core_cnt);                             /*核数量*/
    fprintf(stderr, "rfc->rule_cnt  :%u\n", rfcp->rule_cnt);                             /*实际规则数*/
    fprintf(stderr, "rule in store\n");
    fprintf(stderr, "all rules:\n");
    for(i = 0; i < rfcp->rule_cnt; i++){
            fprintf(stderr, "rule eqid %u : val %u\n",i, rfcp->keyval_map[i]);
            debug_rule(rfcp, rfcp->rule + i);
    }
};

/**
 * @brief 以字节为单位打印特征码
 *
 */
void debug_con(struct rfc *rfcp, void *con){
    u_int8_t *ptr;
    u_int32_t i;
    ptr = (u_int8_t *)con;
    for(i = 0; i < rfcp->con_len; i++){
          fprintf(stderr, "%d ", ptr[i]);     
    }
    fprintf(stderr, "\n");
}



/**
 * @brief 打印规则
 *
 */
void debug_rule(struct rfc *rfcp, struct rfc_rule *rulep){
    fprintf(stderr, "param0:\n");
    debug_con(rfcp, rulep->con[0]);
    fprintf(stderr, "param1:\n");
    debug_con(rfcp, rulep->con[1]);
}


/**
 * @brief 调试解析接口
 *
 */
void debug_parse(struct parse_info *pinfo){
    u_int8_t  *val1, *val2;
    fprintf(stderr, "pinfo:\n");
    fprintf(stderr, "pinfo->pidx: %u\n", pinfo->pidx);                    
    fprintf(stderr, "pinfo->pval: %u\n", pinfo->pval);                    
    fprintf(stderr, "pinfo->pidx: %u\n", pinfo->eqid);                    
    fprintf(stderr, "pinfo->pidx: %u\n", pinfo->bit_len);     
    val1 = (u_int8_t *)(&(pinfo->pval));
    val2 = val1 + 1;
    fprintf(stderr, "pinfo->pval[0]: %u\n", *val1);     
    fprintf(stderr, "pinfo->pval[1]: %u\n", *val2);     
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
}



