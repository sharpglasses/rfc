#ifndef  RFC_H_
#define  RFC_H_
/**
 * @brief rfc manager
 * @file  rfc.h
 * @author mhw
 * @date 2014/3/12
 * @TODO 重构为支持并行计算
 */

#include "rfc_structure.h"
#include "rfc_debug.h"
#include "rfc_parse.h"


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
        void (*pdestory)(void *), void * (*talloc)(u_int32_t), void   (*tdestory)(void *));



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
s_int32_t rfc_read_rule(struct rfc* rfcp, void *con_code1, void *con_code2, u_int32_t val);


/**
 * @breif 构建rfc结构
 * @return 0 成功
 * @return    
 *        
 *
 */
s_int32_t rfc_build(struct rfc *rfcp);


/**
 *
 * @brief 匹配规则
 * @param[in] rfcp rfc ptr
 * @param[in] con_code 特征码
 * @return 可以匹配上多条,因此是一段buff
 *
 */
u_int32_t *rfc_match(struct rfc *rfcp, void *con_code);



/**
 * @breif 销毁rfc结构
 *
 */
void destory_rfc(struct rfc *rfcp);



/**
 * @breif debug_rfc
 *
 */
void debug_rfc(struct rfc *rfcp);





#endif















