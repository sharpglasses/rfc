#ifndef  RFC_PARSE_H_
#define  RFC_PARSE_H_
/**
 * @brief rfc parse condition code
 * @file rfc_parse.h
 * @author mhw
 * @date 2014/3/13
 *
 */
#include "rfc_structure.h"

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
extern s_int32_t piece_parse_rule(u_int32_t pidx, struct rfc *rfcp, struct agg_master *agmp, void *input, void *output,\
        s_int32_t (*callback)(struct rfc *, struct parse_info *, void*, void *));

#endif
