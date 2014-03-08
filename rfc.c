#include "rfc.h"

/**
 * @brief rfc manager
 * @file  rfc.h
 * @author mhw
 * @date 2014/3/12
 * @TODO 重构为支持并行计算
 */

/**
 * @brief 初始化rfc相关
 * @param[in] rule_max 最大规则数
 * @param[in] col_len 特征码长度,字节为单位
 * @param[in] core_cnt 核数量
 * @param[in] palloc 常驻内存申请接口
 * @param[in] pdestory 常驻内存释放接口
 * @param[in] talloc 临时内存申请接口
 * @param[in] tdestory 临时内存释放接口
 */
struct rfc * alloc_rfc(u_int32_t rule_max, u_int32_t con_len, u_int32_t core_cnt, void * (*palloc)(u_int32_t),\
        void (*pdestory)(void *), void * (*talloc)(u_int32_t), void   (*tdestory)(void *))
{
        struct rfc *rfcp = (struct rfc*)palloc(sizeof(struct rfc));
        if(!rfcp){
            return 0;
        }
        rfcp->status = RFC_INIT;
        memset(rfcp, 0, sizeof(struct rfc));
        rfcp->rule_max = rule_max;
        rfcp->con_len  = con_len;
        rfcp->piece_cnt= con_len/2 + con_len%2;
        rfcp->core_cnt = core_cnt;
        rfcp->palloc   = palloc;
        rfcp->pdestory = pdestory;
        rfcp->talloc   = talloc;
        rfcp->tdestory = tdestory;

        rfcp->rbp = alloc_rbp(palloc, pdestory);
        if(!rfcp->rbp){
            return 0;
        }

        rfcp->entry  = (struct ces_entry *)palloc(sizeof(struct ces_entry) * rfcp->piece_cnt);
        if(!rfcp->entry){
            return 0;
        }
        memset(rfcp->entry, 0, sizeof(struct ces_entry) * rfcp->piece_cnt);

        rfcp->rule      = (struct rfc_rule *)talloc(sizeof(struct rfc_rule)*rule_max);
        if(!rfcp->rule){
            return 0;
        }
        memset(rfcp->rule, 0, sizeof(struct rfc_rule)*rule_max);
        rfcp->status = RFC_READ;
        return rfcp;
}


/**
 * @breif 读入规则
 * @param[in] rfcp rfc ptr
 * @param[in] con_code1 特征码1维
 * @param[in] con_code2 特征码2维
 * @param[in] val       特征码对应的值
 * @note
 *       二维特征码扩展及分片方式
 *       扩展: 8bit 扩占
 *       分片: 16bit 优先，剩余到的按照 8bit 分片 
 *
 */
s_int32_t rfc_read_rule(struct rfc* rfcp, void *con_code1, void *con_code2, u_int32_t val)
{
    rfcp->rule[rfcp->rule_cnt].con[0] = con_code1;
    rfcp->rule[rfcp->rule_cnt].con[1] = con_code2;
    if(rbp_set(rfcp->rbp, rfcp->rule_cnt, val) < 0){  /*红黑树内存分配失败检查*/
        return RFC_ERROR;
    }
    rfcp->rule_cnt++;
    return RFC_SUCCESS;
}



/**
 *
 * @brief 匹配规则
 * @param[in] rfcp rfc ptr
 * @param[in] con_code 特征码
 * @return 可以匹配上多条,因此是一段buff
 *
 */
u_int32_t *rfc_match(struct rfc *rfcp, void *con_code)
{


}


/**
 * @breif 销毁rfc结构
 *
 */
void destory_rfc(struct rfc *rfcp)
{


}



/**
 * @breif debug_rfc
 *
 */
void debug_rfc(struct rfc *rfcp)
{
    switch(rfcp->status){
        case RFC_INIT:
        case RFC_READ:
        debug_rfc_read(rfcp);
        break;
        RFC_PHASE0:
        RFC_PHASE1:
        RFC_PHASE2:
        RFC_PHASE3:
        RFC_SEARC:
        RFC_DESTORY:
    default:
        fprintf(stderr, "error case\n");
    }
}

