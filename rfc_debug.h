#ifndef  RFc_DEBUG_H_
#define  RFc_DEBUG_H_

#include "rfc_structure.h"

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
void debug_rfc_read( struct rfc *rfcp);



/**
 * @brief 以字节为单位打印特征码
 *
 */
void debug_con(struct rfc *rfcp, void *con);


/**
 * @brief 打印规则
 *
 */
void debug_rule(struct rfc *rfcp, struct rfc_rule *rulep);

/**
 * @brief 用于调试解析接口
 *
 */
void debug_pinfo(struct parse_info *pinfo);

#endif
