/**
 * @brief rfc build
 * @file rfc_build.h
 * @author mhw
 * @note
 * @date 2014/3/4
 *
 */

#include "rfc_build.h"

/**
 * @brief init init build element
 * @param[in] rfc_mem_param     mem manager
 * @param[in] rfc_bitmap_param  bitmap manager
 * @param[in] rfc_parse_param   parse manager
 * @return rfc_build
 *
 */
struct rfc_build *init_rfc_build(struct rfc_mem *rfc_mem_param, struct rfc_bitmap *rfc_bitmap_param, struct rfc_parse *rfc_parse_param)
{
    struct rfc_build *ret_rfc_build = (struct rfc_build *)rfc_mem_param->tmp_alloc(sizeof(struct rfc_build));
    if(!ret_rfc_build)
        goto rfc_build_fail;
    memset(ret_rfc_build, 0 , sizeof(struct rfc_build));

    ret_rfc_build->slot_table_manager = (struct slot_table *)rfc_mem_param->tmp_alloc(sizeof(struct slot_table));
    if(!ret_rfc_build->slot_table_manager)
        goto rfc_slot_fail;
    memset(ret_rfc_build->slot_table_manager, 0 , sizeof(struct slot_table));

    ret_rfc_build->slot_table_manager->valid = (void **)rfc_mem_param->tmp_alloc(sizeof(void *) * rfc_parse_param->size);
    if(!ret_rfc_build->slot_table_manager)
        goto rfc_table_fail;
    memset(ret_rfc_build->slot_table_manager->valid, 0 , sizeof(void *) * rfc_parse_param->size);

    ret_rfc_build->rfc_mem_manager                  = rfc_mem_param;
    ret_rfc_build->rfc_bitmap_manager               = rfc_bitmap_param;
    ret_rfc_build->rfc_parse_manager                = rfc_parse_param;
    ret_rfc_build->slot_table_manager->slot_cnt     = rfc_parse_param->size;

    return ret_rfc_build;

rfc_table_fail:
    rfc_mem_param->tmp_free(ret_rfc_build->slot_table_manager->valid);
rfc_slot_fail:
    rfc_mem_param->tmp_free(ret_rfc_build->slot_table_manager);
rfc_build_fail:
    rfc_mem_param->tmp_free(ret_rfc_build);
    return 0;
}



/**
 * @brief init destory  build element
 * @param[in] rfc_build_param     rfc_build manager
 * @return
 */
s_int32_t deinit_rfc_build(struct rfc_build *rfc_build_param)
{
    rfc_build_param->rfc_mem_manager->tmp_free(rfc_build_param->slot_table_manager->valid);
    rfc_build_param->rfc_mem_manager->tmp_free(rfc_build_param->slot_table_manager);
    rfc_build_param->rfc_mem_manager->tmp_free(rfc_build_param);
    return 0;
}


/**
 * @brief callback for parse rules in graph mode and build ces, set flag for phase0
 * @param[in] rfc_build_param build manager for this core
 * @param[in] val val parse for condition code
 * @param[in] arg ptr to eqid
 * @return
 */
s_int32_t rfc_graph_phase0_callback(struct rfc_build *rfc_build_param, u_int32_t col, u_int16_t val, u_int32_t eqid, void *arg1, void *arg2){
    //struct rfc_mem          *mem_manager       = NULL;
    //struct rfc_bitmap       *bitmap_manager    = NULL;
    //struct parase_manager   *parse_manager     = NULL;
    struct slot_table       *slot_manager      = NULL;
    //struct ces_table        *ces_table_manager = NULL;

    //mem_manager             = rfc_build_param->rfc_mem_manager;
    //bitmap_manager          = rfc_build_param->rfc_bitmap_manager;
    //parse_manager           = rfc_build_param->rfc_parse_manager;
    //slot_manager            = rfc_build_param->slot_table_manager;

    ces_table =   (struct ces_table *)slot_manager->valid[col];
    ces_table->valid[val]   =  1;                                   /*set flag int ces table for phase1*/
    return 0;
}


/**
 * @brief phase0 for build
 *  1.we create ces table
 *
 *
 * @param[in] rfc_build_manager    build manager for rfc_graph_phase0
 * @param[in] rfc_rule_param       ptr to all rule
 * @param[in] col_start;           slot index start;
 * @param[in] col_end;             slot index end;
 * @return
 * @note multi core  support here by col_start and col_end;
 */
s_int32_t rfc_graph_phase0(struct rfc_build *rfc_build_param, rfc_rule *rfc_rule_param, u_int32_t col_start, u_int32_t col_end){
    u_int32_t               i, j;
    u_int32_t               *valid             = NULL;
    struct ces_entry        *ces_entry_manger  = NULL;

    struct rfc_mem          *mem_manager       = NULL;
    struct parase_manager   *parse_manager     = NULL;
    struct slot_table       *slot_manager      = NULL;
    struct ces_table        *ces_table_manager = NULL;

    mem_manager             = rfc_build_param->rfc_mem_manager;
    parse_manager           = rfc_build_param->rfc_parse_manager;
    slot_manager            = rfc_build_param->slot_table_manager;

    for(col = col_start; col <= col_end; col++){
            ces_table_manager = alloc_ces_table(rfc_build_param, 65535u);
            if(!ces_table_manager){
                return -1;
            }

            ces_table_manager->valid    = (u_int32_t *)mem_manager->tmp_alloc(sizeof(u_int32_t) * ces_table_manager->ces_cnt);
            if(!ces_table_manager->valid){
                return -1;
            }
            memset(ces_table_manager->valid, 0, sizeof(u_int32_t)*ces_table_manager->ces_cnt);

            slot_manager->valid[col]      = (void*)ces_table_manager;       /*create org ces table */
            slot_manager->slot_cnt++;

            /*set flag for ces table*/
            if(col_parse_rule(rfc_rule_param, rfc_build_param, col, 0, rfc_bitmap->rule_nums - 1, NULL, NULL, rfc_graph_prepare_callback) < 0){
                return -1;
            }
            slot_manager->valid[col]      = (void*)ces_table_manager;       /*create org ces table */
    }
    return 0;
}


/**
 * @brief phase1 for build
 *  1.create cbm_table for ces_entry
 * @param[in] rfc_build_manager    build manager for rfc_graph_phase0
 * @param[in] rfc_rule_param       ptr to all rule
 * @param[in] col_start;           slot index start;
 * @param[in] col_end;             slot index end;
 * @return
 * @note multi core  support here by col_start and col_end;
 */
s_int32_t rfc_graph_phase1(struct rfc_build *rfc_build_param, rfc_rule *rfc_rule_param, u_int32_t col_start, u_int32_t col_end){
    u_int32_t               i, j, abmp_id;
    u_int32_t               *tmp_cbm           = NULL;
    struct rfc_bitmap       *bitmap_manager= NULL;
    struct agg_bitmap       *abmp               = NULL;
    struct rfc_mem          *mem_manager       = NULL;
    struct parase_manager   *parse_manager     = NULL;
    struct slot_table       *slot_manager      = NULL;
    struct ces_table        *ces_table_manager = NULL;
    struct cbm_table        *cbm_table_manager = NULL;
    bitmap_manager          = rfc_build_param->rfc_bitmap_manager;
    mem_manager             = rfc_build_param->rfc_mem_manager;
    parse_manager           = rfc_build_param->rfc_parse_manager;
    slot_manager            = rfc_build_param->slot_table_manager;
    tmp_cbm    = (u_int32_t *)mem_manager->tmp_alloc(sizeof(u_int32_t)*bitmap_manager->bitmap_len);
    for(col = col_start; col <= col_end; col++){
                ces_table_manager = (struct ces_table*)slot_table->valid[col];
                cbm_table_manager = alloc_cbm_table(rfc_build_param, rfc_bitmap_manager->rule_nums);
                if(!cbm_table_manager)
                    return -1;
                abmp_id = 0;
                for(i = 0; i < ces_table_manager->ces_cnt; i++){
                        if(ces_table_manager->valid[i]){
                                memset(tmp_cbm, 0, sizeof(u_int32_t)*bitmap_manager->bitmap_len);
                                /*build cbm_map*/
                                if(col_parse_rule(rfc_rule_param, rfc_build_param, col, 0, bitmap_manager->rule_nums - 1, \
                                        (void *)&ces_entry_manager->key[i], (void *)tmp_cbm, rfc_phase1_callback) < 0){
                                    return -1;
                                }
                                abmp = aggragate_bmp_convert(bitmap_manager, tmp_bmp)
                                if(!abmp){
                                    return -1;
                                }
                                abmp_id = search_cbm_table(rfc_bitmap_param, cbm_table_manager, 0, cbm_table_manager->cbm_cnt, abmp);

                                if(abmp_id < 0){                  /*need new one */
                                    if(cbm_table_manager->cbm_cnt >= 65535){
                                    return ph2_fail;    
                                }
                                if(save_bmp(bitmap_manager, abmp) < 0){
                                    return ph2_fail;
                                }
                                abmp_id = cbm_ret->cbm_cnt;
                                cbm_table_manager->valid[abmp_id] = abmp;
                                cbm_table_manager->cbm_cnt++;
                                }
                                ces_table_manager->valid[i] = abmp_id;
                        }
                }
                ces_table_manager->next = (struct ces_table *)cbm_table_manager; /*ces table pointer to the cbm table before build*/
    }
}

/**
 * @brief callback for parse rules in graph mode and build cbm for phase1
 * @param[in] rfc_build_param build manager for this corea
 * @param[in] col col val in condition code (16bit/step)
 * @param[in] val piece val parse from condition code
 * @param[in] arg ptr to eqid
 * @return
 */
s_int32_t rfc_graph_phase1_callback(struct rfc_build *rfc_build_param, u_int32_t col, u_int16_t val, u_int32_t eqid, void *arg1, void *arg2){
    key                     = *((u_int32_t *)arg1);
    tmp_bmp                 = *((u_int32_t *)arg2);
    if(val == key){
        /*set pos in bitmap*/
        tmp_cbm + (eqid>>5) |=(u_int32_t)(1<<(eqid&0x1f));
    }
    return 0;
}


struct phase2_element rfc_graph_phase2_add( struct rfc_build *rfc_build_param, struct phase2_element ph2_lft,  struct phase2_element ph2_rht)
{
    struct phase2_element ph2_ret, ph2_fail;
    u_int32_t               i, j, k, ces_cnt, abmp_id;
    u_int32_t               *abmp               = NULL;
    struct ces_entry        *ces_lft            = NULL;
    struct cbm_entry        *cbm_lft            = NULL;
    struct ces_entry        *ces_rht            = NULL;
    struct cbm_entry        *cbm_rht            = NULL;
    struct ces_entry        *ces_ret            = NULL;
    struct cbm_entry        *cbm_ret            = NULL;
    struct rfc_bitmap       *bitmap_manager     = NULL;
    struct agg_bitmap       *abmp               = NULL;
    struct rfc_mem          *mem_manager        = NULL;
    struct parase_manager   *parse_manager      = NULL;
    struct slot_table       *slot_manager       = NULL;
    struct ces_table        *ces_table_manager  = NULL;

    memset(ph2_fail, 0 , sizeof(struct phase2_element));
    bitmap_manager          = rfc_build_param->rfc_bitmap_manager;
    mem_manager             = rfc_build_param->rfc_mem_manager;
    parse_manager           = rfc_build_param->rfc_parse_manager;
    ces_lft    = ph2_lft->ces_table_ptr;
    cbm_lft    = ph2_lft->cbm_table_ptr;
    ces_rht    = ph2_rht->ces_table_ptr;
    cbm_rht    = ph2_rht->cbm_table_ptr;

    ces_cnt    = cbm_lft->cbm_cnt * cbm_rht->cbm_cnt;
    ces_ret    = alloc_ces_table(rfc_build_param, u_int32_t ces_cnt);
    if(!ces_ret){
        return ph2_fail;
    }
    cbm_ret    = alloc_cbm_table(rfc_build_param, bitmap_manager->rule_nums);
    if(!cbm_ret){
        return ph2_fail;
    }
    abmp_id = 0;
    for(int i = 0; i < cbm_lft->cbm_cnt; i++)
    for(int j = 0; j < cbm_lft->cbm_cnt; j++){
                k = cbm_lft->cbm_cnt*i + cbm_rht->cbm_cnt;
                abmp = aggrate_bmp_or(rfc_bitmap_param, cbm_lft->valid[i], cbm_rht->valid[j]);
                if(!abmp){
                    return ph2_fail;
                }

                abmp_id = search_cbm_table(bitmap_manager, cbm_ret, abmp);
                if(abmp_id < 0){                  /*need new one */
                    if(cbm_ret->cbm_cnt >= 65535){
                        return ph2_fail;    
                    }
                    if(save_bmp(bitmap_manager, abmp) < 0){
                        return ph2_fail;
                    }
                    abmp_id = cbm_ret->cbm_cnt;
                    cbm_ret->valid[abmp_id] = abmp;
                    cbm_ret->cbm_cnt++;
                }
                ces_ret->valid[k] = abmp_id;
        }

    }
}


/**
 * @brief phase2 for build
 *  1.create all ces_table
 *  2.destory all cbm_table but last one
 * @param[in] rfc_build_manager    build manager for rfc_graph_phase0
 * @param[in] rfc_rule_param       ptr to all rule
 * @param[in] col_start;           slot index start;
 * @param[in] col_end;             slot index end;
 * @return
 * @note multi core  support here by col_start and col_end;
 */
struct phase2_element rfc_graph_phase2(struct rfc_build *rfc_build_param, u_int32_t col_start, u_int32_t col_end){
    struct slot_table       *slot_manager      = NULL;
    u_int32_t mid;
    struct phase2_element ph2_ret, ph2_lft, ph2_rht, ph2_fail;
    memset(ph2_fail, 0 , sizeof(struct phase2_element));

    if(col_start == col_end){
            ph2_ret =  alloc_phase2_element(rfc_build_param);
            if(!ph2_ret){
                return 0;
            }
            slot_manager            = rfc_build_param->slot_table_manager;
            ph2_ret->ces_table_ptr  = slot_manager->valid[col_start];
            ph2_ret->cbm_table_ptr  = (struct cbm_table *)slot_manager->valid[col_start]->next;
            return  ph2_ret;
    }else{
            mid = (start + end)/2;

            ph2_lft = rfc_graph_phase2(rfc_build_param, col_start, mid); 
            ph2_rht = rfc_graph_phase2(rfc_build_param, mid + 1,   col_end);

            if(0 == ph2_lft.ces_table_Ptr || ph2_rht.ces_table_ptr|| 0 == ph2_lft.cbm_table_Ptr || ph2_rht.cbm_table_ptr){ 
                /*check ces_table ,cbm_table ptr == NIL when fail*/
                        return ph2_fail;
            }

            ret_ptr = rfc_graph_phase2_add(ph2_lft, ph2_rht);

            if(0 == ph2_ret.ces_table_Ptr || ph2_ret.ces_table_ptr){ 
                /*check ces_table ptr == NIL when fail*/
                        return ph2_fail;
            }
            if(free_cbm_table(rfc_build_param, ph2_lft->cbm_table_ptr) < 0){
                        return ph2_fail;
            }
            if(free_cbm_table(rfc_build_param, ph2_rht->cbm_table_ptr) < 0){
                        return ph2_fail;
            }

            ph2_lft->next = ph2_ret->ces_table_ptr;
            ph2_rht->next = ph2_ret->ces_table_ptr;
    }
}


/**
 * @brief search aggrated bitmap in cbm_table
 * @param[in] rfc_build_manager    build manager for rfc_graph_phase0
 * @param[in] cbm_table_param      cbm_table search
 * @param[in] abmp aggrated bitmap wait to search in cbm table;
 * @return
 */
s_int32_t search_cbm_table(struct rfc_bitmap *rfc_bitmap_param, struct cbm_table *cbm_table_param, struct agg_bitmap *abmp)
{
	s_int32_t i;
	for(i= 0; i< cbm_table_param->cbm_cnt; i++)
	{
                if( 0 == aggrate_bmp_comp(rfc_bitmap_param, cbm_table_param->valid[i], abmp))
			return i;
	}
	return -1;
}



/**
 * @brief alloc ces_entry
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] entry_cnt size of the entry
 * @return ces_entry ptr
 */
struct  ces_entry *alloc_ces_entry(struct rfc_build *rfc_build_param, u_int32_t entry_cnt){
        struct rfc_mem          *mem_manager       = NULL;
        struct ces_entry        *ces_entry_manger  = NULL;

        mem_manager                 = rfc_build_param->rfc_mem_manager;

        ces_entry_manager           = (struct ces_entry *)mem_manager->permanent_alloc(sizeof(struct ces_entry));
        if(!ces_entry_manager){
            return 0;
        }
        memset(ces_entry_manager, 0, sizeof(struct ces_entrt_manager));

        ces_entry_manager->key      = (u_int32_t *)mem_manager->permanent_alloc(sizeof(u_int32_t)*entry_cnt);
        if(!ces_entry_manager->key){
            return 0;
        }
        memset(ces_entry_manager->key, 0, sizeof(u_int32_t)*entry_cnt);

        ces_entry_manager->entry    = (u_int32_t *)mem_manager->permanent_alloc(sizeof(u_int32_t)*entry_cnt);
        if(!ces_entry_manager->entry){
            return 0;
        }
        memset(ces_entry_manager->entry, 0, sizeof(u_int32_t)*entry_cnt);

    return ces_entry_manager;
}


/**
 * @brief free ces_entry
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] ces_entry_manager ptr to the ces entry
 *
 */
s_int32_t free_ces_entry(struct rfc_build * rfc_build_param, struct ces_entry *ces_entry_manager){
        struct rfc_mem              *mem_manager       = NULL;
        mem_manager                 = rfc_build_param->rfc_mem_manager;
        mem_manager->permanent_free(ces_entry_manager->key);
        mem_manager->permanent_free(ces_entry_manager->entry);
        mem_manager->permanent_free(ces_entry_manager);
    return 0;
}



/* @brief alloc ces_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] ces_cnt size of the table
 * @return ces_table ptr
 * @note valid not alloc
 */
struct  ces_table *alloc_ces_table(struct rfc_build *rfc_build_param, u_int32_t ces_cnt){
        struct rfc_mem          *mem_manager        = NULL;
        struct ces_table        *ces_table_manager  = NULL;

        mem_manager                 = rfc_build_param->rfc_mem_manager;

        ces_table_manager           = (struct ces_table *)mem_manager->permanent_alloc(sizeof(struct ces_table));
        if(!ces_table_manager){
            return 0;
        }
        memset(ces_table_manager, 0, sizeof(struct ces_table));

        ces_table_manager->valid    = (u_int32_t *)mem_manager->permanent_alloc(sizeof(u_int32_t)*entry_cnt);
        ces_table_manager->ces_cnt  = ces_cnt;
    return ces_table;
}



/**
 * @brief free ces_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] ces_table_manager ptr to the ces table
 * @note valid not free
 *
 */
s_int32_t free_ces_table(struct rfc_build * rfc_build_param, struct ces_table *ces_table_manager){
        struct rfc_mem              *mem_manager       = NULL;
        mem_manager                 = rfc_build_param->rfc_mem_manager;
        mem_manager->permanent_free(ces_table_manager->valid);
        mem_manager->permanent_free(ces_table_manager);
    return 0;
}



/* @brief alloc ces_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] ces_cnt size of the table
 * @return ces_table ptr
 */
struct  ces_table *alloc_ces_table(struct rfc_build *rfc_build_param, u_int32_t ces_cnt){
        struct rfc_mem          *mem_manager        = NULL;
        struct ces_table        *ces_table_manager  = NULL;

        mem_manager                 = rfc_build_param->rfc_mem_manager;

        ces_table_manager           = (struct ces_table *)mem_manager->permanent_alloc(sizeof(struct ces_table));
        if(!ces_table_manager){
            return 0;
        }
        memset(ces_table_manager, 0, sizeof(struct ces_table));

        ces_table_manager->valid    = (u_int32_t *)mem_manager->permanent_alloc(sizeof(u_int32_t)*entry_cnt);
        if(!ces_table_manager->valid){
            return 0;
        }
        memset(ces_table_manager->valid, 0, sizeof(u_int32_t)*entry_cnt);

        ces_table_manager->ces_cnt  = ces_cnt;
    return ces_table;
}



/**
 * @brief free ces_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] ces_table_manager ptr to the ces table
 *
 */
s_int32_t free_ces_table(struct rfc_build * rfc_build_param, struct ces_table *ces_table_manager){
        struct rfc_mem              *mem_manager       = NULL;
        mem_manager                 = rfc_build_param->rfc_mem_manager;
        mem_manager->permanent_free(ces_table_manager->valid);
        mem_manager->permanent_free(ces_table_manager);
    return 0;
}



/* @brief alloc cbm_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] cbm_cnt size of the table
 * @return cbm_table ptr
 */
struct  cbm_table *alloc_cbm_table(struct rfc_build *rfc_build_param, u_int32_t cbm_cnt){
        struct rfc_mem          *mem_manager        = NULL;
        struct cbm_table        *cbm_table_manager  = NULL;

        mem_manager                 = rfc_build_param->rfc_mem_manager;

        cbm_table_manager           = (struct cbm_table *)mem_manager->tmp_alloc(sizeof(struct cbm_table));
        if(!cbm_table_manager){
            return 0;
        }
        memset(cbm_table_manager, 0, sizeof(struct cbm_table));

        cbm_table_manager->valid    = (u_int32_t *)mem_manager->tmp_alloc(sizeof(struct agg_bitmap*)*cbm_cnt);
        if(!cbm_table_manager->valid){
            return 0;
        }
        memset(cbm_table_manager->valid, 0, sizeof(struct agg_bitmap*)*cbm_cnt);

        cbm_table_manager->cbm_cnt  = cbm_cnt;
    return cbm_table;
}



/**
 * @brief free cbm_table
 * @param[in] rfc_build_param ptr to the build manager
 * @param[in] cbm_table_manager ptr to the cbm table
 * @TODO free all aggrate_bitmap here;
 */
s_int32_t free_cbm_table(struct rfc_build * rfc_build_param, struct cbm_table *cbm_table_manager){
        struct rfc_mem              *mem_manager       = NULL;
        mem_manager                 = rfc_build_param->rfc_mem_manager;
        mem_manager->tmp_free(cbm_table_manager->valid);
        mem_manager->tmp_free(cbm_table_manager);
    return 0;
}






