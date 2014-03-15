#ifndef RFC_CREATE_H_
#define RFC_CREATE_H_
/**
 * @brief rfc create.h
 * @file rfc_create.h
 * @author mhw
 * @note
 * @date 2014/3/15
 *
 */
#include "rfc_structure.h"

/**
 * @brief alloc ces_table
 * @param[in] rfcp struct rfc 
 * @param[in] ces_cnt struct ces table size 
 * @return ces_table ptr
 */
extern struct  ces_table *alloc_ces_table(struct rfc *rfcp, u_int32_t ces_cnt);



/**
 * @brief destory ces_table
 * @param[in] rfcp struct rfc 
 * @param[in] ces_table ptr to the ces_table
 *
 */
extern void destory_ces_table(struct rfc * rfcp, struct ces_table * cetp);

/**
 * @brief set flag for ces
 * @param[in] rfcp ptr to rfc
 * @param[in] agmp ptr to agg_bitmap_master
 * @param[in] spidx start of the pidx
 * @param[in] epdix end of the pidx
 * @return 
 */
extern s_int32_t phase0_build(struct rfc *rfcp, struct agg_master *agmp, u_int32_t pidx);



/**
 * @brief build phase1
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] pidx index for piece
 * @return
 */
extern s_int32_t phase1_build(struct rfc *rfcp, struct agg_master *agmp, u_int32_t pidx);



/**
 * @brief build phase0 ~ phase1
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] begin_pidx begin index for piece
 * @param[in] end_pidx end index for piece
 * @return
 */
extern s_int32_t rfc_build_prepare(struct rfc *rfcp, struct agg_master *agmp, u_int32_t begin_pidx, u_int32_t end_pidx);

#endif
