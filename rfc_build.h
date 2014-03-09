#ifndef  RFC_BUILD_H_
#define  RFC_BUILD_H_

/**
 * @brief rfc build
 * @file rfc_build.h
 * @author mhw
 * @note 
 * @date 2014/3/12
 *
 */
#include "rfc_structure.h"
#include "rfc_parse.h"

struct ph2_ret{
    struct ces_table *cetp;     
    struct cbm_table *cbtp;
};

/**
 * @breif param[in] 子构建单元, 并行构建的分配接口
 * @param[in] rfc rfcp 全局管理结构
 * @param[in] agmp 聚合表管理，每个计算单元一个实体
 * @param[in] sidx 构建集合开始索引
 * @param[in] eidx 构建集合借宿索引
 *
 */
extern struct rfc_build *rfc_build_child(struct rfc *rfcp, struct agg_master *agmp, u_int32_t sidx, u_int32_t eidx);


#endif
