/**
 * @brief rfc parse condition code
 * @file rfc_parse.c
 * @author mhw
 * @date 2014/3/12
 *
 */
#include "rfc_parse.h"


/**
 * @brief 遍历所有rule的某一个piece
 * @param[in] pidx   被分析的分片索引
 * @param[in] rfcp   rfc 管理结构
 * @param[in] agmp   聚合表管理，在启动并行计算时，该结构对每一个线程分配一个实体
 * @param[in] input  外部输入结构，用于回调使用
 * @param[in] output 外部输出结构, 用于回调函数使用
 * @param[in] callback 回调函数,接收parse 获取的数据以及内外部结构进行分析数据并处理
 * @note 
 *      1.扩展方式为8bit扩展
 *      2.分片方式为先16bit分片
 *      3.逐条扩展防止内存爆炸
 *@TODO 输入16bit 对齐限制
 */
s_int32_t piece_parse_rule(u_int32_t pidx, struct rfc *rfcp, struct agg_master *agmp, void *input, void *output,\
        s_int32_t (*callback)(struct rfc *, struct parse_info *, void*, void *))
{
    u_int16_t *con1;
    u_int16_t *con2;
    u_int8_t  *con11, *con12;
    u_int8_t  *con21, *con22;
    u_int8_t  *val1, *val2;
    struct parse_info pinfo;
    val1 = (u_int8_t *)(&(pinfo.pval));
    val2 = val1 + 1;
    u_int32_t  i;
    u_int32_t  j;

    pinfo.pidx = pidx;
    struct rfc_rule * rulp = rfcp->rule;                         /*point at he header of the rule table*/
    pinfo.bit_len = 16;                                          /* 暂时未使用*/                                                 
    for(pinfo.eqid = 0; pinfo.eqid < rfcp->rule_cnt; pinfo.eqid++){                /*遍历规则*/
        con1 = (u_int16_t *)rulp[pinfo.eqid].con[0];                 /* 获得特征码*/
        con2 = (u_int16_t *)rulp[pinfo.eqid].con[1];
        
        con1 += pidx;
        con2 += pidx;

        con11 = (u_int8_t *)con1;
        con12 = con11 + 1;

        con21 = (u_int8_t *)con2;
        con22 = con21 + 1;
        if(*con11 > *con21 || *con12 > *con22){
            return -1;
        }
        for(i = *con11; i <= *con21; i++)
            for(j = *con12; j <= *con22; j++){
                    *val1 = i;
                    *val2 = j;
                    if(callback(rfcp, &pinfo, input, output) < 0){
                        return -1;
                    }
        }
        rulp++;
    }
}

























