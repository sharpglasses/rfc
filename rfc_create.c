/**
 * @brief rfc create.c
 * @file rfc_create.c
 * @author mhw
 * @note
 * @date 2014/3/15
 *
 */
#include "rfc_create.h"
#include "rfc_parse.h"


s_int32_t phase0_callback(struct rfc *rfcp, struct parse_info *pinfo, void *input, void *output);
s_int32_t phase1_callback(struct rfc *rfcp, struct parse_info *pinfo, void *input, void *output);
struct phase2_link phase2_add(struct rfc *rfcp, struct agg_master *agmp, struct phase2_link *ph2_lft, struct phase2_link *ph2_rht);

/**
 * @brief alloc ces_table
 * @param[in] rfcp struct rfc 
 * @param[in] ces_cnt struct ces table size 
 * @return ces_table ptr
 */
struct  ces_table *alloc_ces_table(struct rfc *rfcp, u_int32_t ces_cnt){
        struct ces_table * cetp = (struct ces_table *)rfcp->palloc(sizeof(struct ces_table)); 
        if(!cetp){
            return 0;
        }
        memset(cetp, 0, sizeof(struct ces_table));

        cetp->table = (u_int16_t *)rfcp->palloc(sizeof(u_int16_t) * ces_cnt); 
        if(!cetp->table){
            return 0;
        }
        memset(cetp->table, 0, sizeof(u_int16_t) * ces_cnt);
    return cetp;
}

/**
 * @brief destory ces_table
 * @param[in] rfcp struct rfc 
 * @param[in] ces_table ptr to the ces_table
 *
 */
void destory_ces_table(struct rfc * rfcp, struct ces_table * cetp){
        rfcp->pdestory(cetp->table);
        rfcp->pdestory(cetp);
}

/**
 * @brief callback for parse rules and build ces_table
 * @param[in]  rfcp ptr to 
 * @param[in]  pinfo ptr get from parse
 * @param[in]  input input val ptr
 * @param[in]  output output val ptr 
 * @note set flag for ces table
 * return 
 */
s_int32_t phase0_callback(struct rfc *rfcp, struct parse_info *pinfo, void *input, void *output){
    rfcp->entry[pinfo->pidx]->table[pinfo->pval] = 1;
    return 0;
}



/**
 * @brief build phase0
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] pidx index for piece
 * @return
 */
s_int32_t phase0_build(struct rfc *rfcp, struct agg_master *agmp, u_int32_t pidx){
    if(piece_parse_rule(pidx, rfcp, agmp, NULL, NULL, phase0_callback) < 0){
        return -1;
    }
    return 0;
}

/**
 * @brief callback for parse rules end build cbm set ces table
 * @param[in]  rfcp ptr to 
 * @param[in]  pinfo ptr get from parse
 * @param[in]  input input val ptr
 * @param[in]  output output val ptr 
 * @note build cbm item
 * return 
 */

s_int32_t phase1_callback(struct rfc *rfcp, struct parse_info *pinfo, void *input, void *output){
    u_int32_t i;    
    u_int32_t *cbm;
    i   = *((u_int32_t*)input);
    cbm = (u_int32_t*)cbm;
    if(i = pinfo->pval){
	cbm[i>>5] |= (u_int32_t)(0x1 << (i&0x1f));
    }
    return 0;
}

/**
 * @brief build phase1
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] pidx index for piece
 * @return
 */
s_int32_t phase1_build(struct rfc *rfcp, struct agg_master *agmp, u_int32_t pidx){
    struct ces_table *cetp;
    struct  agg_table *agtp;
    struct agg_bitmap *abmp;
    u_int32_t eqid;
    u_int32_t *cbm;
    u_int32_t i;
    cetp = rfcp->entry[pidx];
    agtp = alloc_agtp(agmp);
    if(!agtp){
        return -1;
    }
    cetp->next = (struct ces_table*)agtp;  /*point to cbm table tmp */
    cbm =  (u_int32_t *)rfcp->talloc(sizeof(u_int32_t)*agmp->bitmap_len);
    if(!cbm){
        return -1;
    }
    for(i = 0; i < CBM_TABLE_LIMIT; i++){  
        if(cetp->table[i]){                   /*set cbm for all ces items with flag*/
            memset(cbm, 0, sizeof(u_int32_t)*agmp->bitmap_len);
            if(piece_parse_rule(pidx, rfcp, agmp, (void *)&i, (void *)cbm, phase0_callback) < 0){
                return -1;
            }
            abmp = bmp_to_abmp(agmp, cbm);   /*bmp to cbmp*/
            if(!abmp){
                return -1;
            }
            eqid = search_abmp_in_table(agmp, agtp, abmp);
            if( agtp->abmp_cnt == eqid){      /*not found*/
                if(eqid >= CBM_TABLE_LIMIT){
                    return -1;
                }
                abmp  = alloc_abmp(agmp);
                if(!abmp){
                    return -1;
                }
                add_abmp_tail(agtp, abmp);
            }
            cetp->table[i] = eqid;
        }
    }
    return 0;
}

/**
 * @brief build phase0 ~ phase1
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] begin_pidx begin index for piece
 * @param[in] end_pidx end index for piece
 * @return
 */
s_int32_t rfc_build_prepare(struct rfc *rfcp, struct agg_master *agmp, u_int32_t begin_pidx, u_int32_t end_pidx){
    u_int32_t pidx;
    for(pidx = begin_pidx; pidx <= end_pidx; pidx++){
        if(phase0_build(rfcp, agmp, pidx) < 0){
            return -1;
        }
        if(phase1_build(rfcp, agmp, pidx) < 0){
            return -1;
        }
    }
    return 0;
}

/**
 * @brief build phase2
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] begin_pidx begin index for piece
 * @param[in] end_pidx end index for piece
 * @return phase2_link
 */
struct phase2_link phase2_build(struct rfc *rfcp, struct agg_master *agmp, u_int32_t begin_pidx, u_int32_t end_pidx){
    struct phase2_link ph2_ret;
    struct phase2_link ph2_lft;
    struct phase2_link ph2_rht;
    ph2_ret.cetp = 0;   /*set flag*/
    u_int32_t pidx;
    if(begin_pidx = end_pidx){
            ph2_ret.cetp = rfcp->entry[begin_pidx];      
            ph2_ret.agtp = (struct agg_table *)ph2_ret.cetp->next;      
            return ph2_ret;
    }
    pidx = (begin_pidx + end_pidx)/2;
    ph2_lft = phase2_build(rfcp, agmp, begin_pidx, pidx);
    ph2_rht = phase2_build(rfcp, agmp, pidx, end_pidx);
    if(0 == ph2_lft.cetp && 0 == ph2_rht.cetp){
        return ph2_ret;
    }
    //ph2_ret = phase2_add(rfcp, agmp, &ph2_lft, &ph2_rht);
    if(0 == ph2_ret.cetp){
        return ph2_ret;
    }
    destory_agtp(agmp, ph2_lft.agtp);
    destory_agtp(agmp, ph2_rht.agtp);
    ph2_lft.cetp->next = ph2_ret.cetp;
    ph2_rht.cetp->next = ph2_ret.cetp;
}


/**
 * @brief build phase2
 * @param[in] rfc   rfc manager ptr
 * @param[in] agmp ptr for the agg_master
 * @param[in] ph2_lft ptr to phase_link based on
 * @param[in] ph2_rht ptr to phase_link based on
 * @return phase2_link
 */
struct phase2_link phase2_add(struct rfc *rfcp, struct agg_master *agmp, struct phase2_link *ph2_lft, struct phase2_link *ph2_rht)
{
    struct phase2_link ph2_ret; 
    struct list_head  *pos_lft;
    struct list_head  *pos_rht;
    struct agg_bitmap *abmp_lft;
    struct agg_bitmap *abmp_rht;
    struct agg_bitmap *abmp;
    struct agg_table *agtp;
    struct ces_table *cetp;
    u_int32_t ces_cnt;
    u_int32_t i, j;
    ph2_ret.cetp = 0;
    u_int32_t eqid;
    ces_cnt = ph2_lft->cetp->cbm_cnt * ph2_rht->cetp->cbm_cnt;
    cetp = alloc_ces_table(rfcp, ces_cnt);
    if(0 == cetp){
        return ph2_ret;
    }
    agtp = alloc_agtp(agmp);
    if(0 == agtp){
        return ph2_ret;
    }
    i = 0, j = 0;
    list_for_each(pos_lft, &ph2_lft->agtp->abmp_head){
        list_for_each(pos_rht, &ph2_rht->agtp->abmp_head){
            abmp_lft = container_of(pos_lft, struct agg_bitmap, abmp_link);
            abmp_rht = container_of(pos_rht, struct agg_bitmap, abmp_link);
            abmp     = abmp_and(agmp, abmp_lft, abmp_rht);
            eqid = search_abmp_in_table(agmp, agtp, abmp);
            if( agtp->abmp_cnt == eqid){      /*not found*/
                if(eqid >= CBM_TABLE_LIMIT){
                    return ph2_ret;
                }
                abmp  = alloc_abmp(agmp);
                if(!abmp){
                    return ph2_ret;
                }
                add_abmp_tail(agtp, abmp);
            }
            cetp->table[i] = eqid;
            j++;
        }
        i++;
    }
    ph2_ret.cetp = cetp;
    ph2_ret.agtp = agtp;
    return ph2_ret;
}




