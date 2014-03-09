/**
 * @brief rfc aggregate aggbitmap manager
 * @file aggregate_master.c
 * @author mhw
 * @date 2014/3/8
 */
#include "aggregate_master.h"

/**
 * @brief alloc agg_master
 * @param[in] agg_alloc ptr used to alloc mem
 * @param[in] agg_destory ptr used to free mem
 * @param[in] bit_len tot bits of bitmap wait to aggregate
 * return ptr to the agg_master
 */
struct agg_master *alloc_agg_master(void *(*agg_alloc)(u_int32_t), void (*agg_destory)(void *), u_int32_t bit_len){
        struct agg_master *new_agg_master = (struct agg_master *)agg_alloc(sizeof(struct agg_master));
        if(!new_agg_master){
            return 0;
        }
        memset(new_agg_master,  0,  sizeof(struct agg_master));

        INIT_LIST_HEAD(&new_agg_master->table_head);
        new_agg_master->agg_alloc       = agg_alloc;
        new_agg_master->agg_destory     = agg_destory;

        new_agg_master->bit_len         = bit_len;                                  /* 1bit/step   */
        new_agg_master->bitmap_len      = (new_agg_master->bit_len    + 31u) >> 5;  /* 32bit/step */
        new_agg_master->agg_bitmap_len  = (new_agg_master->bitmap_len + 31u) >> 5;  /* 32bit/step */

        new_agg_master->mem = (struct agg_bitmap *)new_agg_master->agg_alloc(ABMP_HEAD_SIZE + 4*new_agg_master->agg_bitmap_len + 4*new_agg_master->bitmap_len);
        if(!new_agg_master->mem){
            return 0;
        }
        new_agg_master->mem_cnt +=  ABMP_HEAD_SIZE + 4*new_agg_master->agg_bitmap_len + 4*new_agg_master->bitmap_len;
        new_agg_master->mem_cnt +=  ABMP_MASTER_SIZE;
        return new_agg_master;
}


/**
 * @brief destory_agg_master
 * @param[in] agmp ptr to the agg_master
 * @return 
 */
void destory_agg_master(struct agg_master *agmp){
        struct agg_table *agg_table_ptr;
        while(agmp->table_cnt){
                agg_table_ptr = container_of(agmp->table_head.next, struct agg_table, table_link);
                destory_agg_table(agmp, agg_table_ptr);
                list_del(&agg_table_ptr->table_link);
                agmp->table_cnt--;
        }
        agmp->agg_destory(agmp->mem);
        agmp->agg_destory(agmp);
        agmp->mem_cnt -=  ABMP_HEAD_SIZE + 4*agmp->agg_bitmap_len + 4*agmp->bitmap_len;
        agmp->mem_cnt -=  ABMP_MASTER_SIZE;
}

/**
 * @brief  save agg_table 
 * @param[in] agmp  ptr to the agg_master 
 * @return  ptr to new agg_table 
 */
struct  agg_table * alloc_agg_table(struct agg_master *agmp){
    struct agg_table *new_agg_table  = (struct agg_table *)agmp->agg_alloc(ABMP_TABLE_SIZE);
    if(!new_agg_table){
        return 0;
    }
    memset(new_agg_table, 0, ABMP_TABLE_SIZE);
    INIT_LIST_HEAD(&new_agg_table->table_link);
    INIT_LIST_HEAD(&new_agg_table->abmp_head);
    list_add(&new_agg_table->table_link, &agmp->table_head);
    agmp->mem_cnt += ABMP_TABLE_SIZE;
    agmp->table_cnt++;
    return 0;
};

/**
 * @brief destory_agg_table
 * @param[in] agtp ptr to the agg_master
 * @return 
 */
void destory_agg_table(struct agg_master *agmp, struct agg_table *agtp)
{
    u_int32_t i;
    struct agg_bitmap *abmp_tmp;
    while(agtp->abmp_cnt){
            abmp_tmp = container_of(agtp->abmp_head.next, struct agg_bitmap, abmp_link);
            remove_agg_bitmap(agtp, abmp_tmp);
            destory_agg_bitmap(agmp, abmp_tmp);
            agtp->abmp_cnt--;
    }
    agmp->agg_destory(agtp);
    agmp->mem_cnt -= ABMP_TABLE_SIZE;
    agmp->table_cnt--;
}

/**
 * @brief alloc space for aggregated bitmap 
 * @param[in] agmp ptr to agg_master
 * @param[in] bmp            ptr to normal bitmap
 * @return ptr to the aggregated bitmap
 */
struct agg_bitmap *alloc_agg_bitmap(struct agg_master *agmp){
    struct agg_bitmap *abmp_tmp;
    u_int32_t x;
    abmp_tmp = (struct agg_bitmap*)agmp->mem;
    x = ABMP_HEAD_SIZE + 4*abmp_tmp->bmp_cnt + 4*agmp->agg_bitmap_len;
    abmp_tmp = (struct agg_bitmap *)agmp->agg_alloc(x);
    if(abmp_tmp){
        return 0;
    }
    memcpy(abmp_tmp, agmp->mem, x);
    agmp->mem_cnt += x;
    agmp->abmp_cnt++;
}

/**
 * @brief alloc space for aggregate
 * @param[in] agmp ptr to agg_master
 * @param[in] abmp ptr to normal bitmap
 * @return 
 */
void destory_agg_bitmap(struct agg_master *agmp, struct agg_bitmap * abmp){
    u_int32_t x; 
    x = ABMP_HEAD_SIZE + 4*abmp->bmp_cnt + 4*agmp->agg_bitmap_len;
    agmp->agg_destory(abmp);
    agmp->mem_cnt -= x;
    agmp->abmp_cnt--;
}

/**
 * @brief add  agg_bitmap into agg_table head
 * @param[in]  agtp  the table want to save this aggregated bitmap
 * @param[in]  abmp the ptr to the aggregated bitmap
 */
void add_agg_bitmap(struct agg_table *agtp, struct agg_bitmap *abmp){
    list_add(&abmp->abmp_link, &agtp->abmp_head);
    agtp->abmp_cnt++;
};

/**
 * @brief add  agg_bitmap into agg_table tail
 * @param[in]  agtp  the table want to save this aggregated bitmap
 * @param[in]  abmp the ptr to the aggregated bitmap
 */
void add_agg_bitmap_tail(struct agg_table *agtp, struct agg_bitmap *abmp){
    list_add_tail(&abmp->abmp_link, &agtp->abmp_head);
    agtp->abmp_cnt++;
};

/**
 * @brief remove agg_bitmap from the agg_table 
 * @param[in]  agtp  the aggregated bitmap in 
 * @param[in]  abmp the ptr to the aggregated bitmap
 */
void remove_agg_bitmap(struct agg_table *agtp, struct agg_bitmap *abmp){
    list_del(&abmp->abmp_link);
    agtp->abmp_cnt--;
};


/**
 * @brief convert normal bitmap to aggregated bitmap 
 * @param[in] agmp ptr to agg_master
 * @param[in] bmp            ptr to normal bitmap
 * @return ptr to the aggregated bitmap
 *
 */
struct agg_bitmap *bmp_to_abmp(struct agg_master *agmp, u_int32_t *bmp)
{
	u_int32_t *p2;
	u_int32_t *p3;
	u_int32_t i;
	u_int32_t detail_bmp_count = 0;
	struct agg_bitmap *abmp = 0;
        memset(agmp->mem, 0, ABMP_HEAD_SIZE + 4*agmp->agg_bitmap_len + 4*agmp->bitmap_len);
	abmp = agmp->mem;

	p2 = (u_int32_t *)(abmp + 1);
	p3 = p2 + agmp->agg_bitmap_len;

	for(i=0; i< agmp->agg_bitmap_len; i++)
	{
		if(bmp[i])
		{
			p2[i>>5] |= (u_int32_t)(0x1 << (i&0x1f));
			*p3 = bmp[i];
			detail_bmp_count++;
			p3++;
		}
	}
	abmp->bmp_cnt = detail_bmp_count;
	return abmp;
}


/**
 * @brief convert aggregated bitmap to normal bitmap 
 * @param[in] agmp ptr to agg_master
 * @param[in] abmp ptr to the aggregated bitmap
 * @param[in] bmp            ptr to normal bitmap
 * @return ptr to the aggregated bitmap
 */
u_int32_t *abmp_to_bmp(struct agg_master * agmp, struct agg_bitmap *abmp, u_int32_t *bmp){
	u_int32_t *p2;
	u_int32_t *p3;
	u_int32_t i, j, k;
	p2 = (u_int32_t *)(abmp + 1);
	p3 = p2 + agmp->agg_bitmap_len;
	k = 0;
	for(i= 0; i< agmp->agg_bitmap_len; i++)
        {
            for(j = 0; j < 32 ; j++){
                if(p2[i] & (1 << j)){
                    bmp[k++] = *p3++;
                }else{
                    bmp[k++] = 0u;
                }
            }     
        
        }
        return bmp;
}

/**
 * @brief get aggregated bitmap by aggregated bitmap1 | aggregated bitmap2
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp1 aggregated bitmap1
 * @param[in] abmp2 aggregated bitmap2
 * @return aggregated bitmap
 */
struct agg_bitmap *abmp_or(struct agg_master *agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2)
{
	u_int32_t i,j;
	u_int32_t *p12,*p13;
	u_int32_t *p22,*p23;
	u_int32_t *abmp;
	struct agg_bitmap *abmp_ret;
	u_int32_t *p2,*p3;
	u_int32_t mask;
	u_int32_t detail_bmp_count = 0;

        memset(agmp->mem, 0, ABMP_HEAD_SIZE + 4*agmp->agg_bitmap_len + 4*agmp->bitmap_len);
	abmp = (u_int32_t *)agmp->mem;

	p12 = (u_int32_t*)(abmp1 + 1);
	p13 = p12 + agmp->agg_bitmap_len;
	p22 = (u_int32_t*)(abmp2 + 1);
	p23 = p22 + agmp->agg_bitmap_len;

	p2 = (u_int32_t*)(abmp +1);
	p3 = p2 + agmp->agg_bitmap_len;

	for(i=0; i< agmp->agg_bitmap_len; i++)
	{
		mask = 0x1;
		for(j=0;j<32;j++)
		{
			if((mask & (*p12) & (*p22)) && ((*p13) & (*p23)))
			{
				*p2 |= mask;
				*p3 = (*p13) & (*p23);
				p3++;
				detail_bmp_count++;
			}
			if(mask & *p12)
				p13++;
			if(mask & *p22)
				p23++;

			mask <<= 1;
		}
		p12++;
		p22++;

	}
	abmp_ret = (struct agg_bitmap*)abmp;
	abmp_ret->bmp_cnt = detail_bmp_count;
	return abmp_ret;
}

/**
 * @brief check if aggregated bmp equal 
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp1 aggregated bitmap1
 * @param[in] abmp2 aggregated bitmap2
 * @return 
 * @note this is faster then aggregated_bmp_cmp
 */
s_int32_t abmp_equal( struct agg_master * agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2)
{
	u_int32_t i,count, *p1, *p2;

	p1 = (u_int32_t*)((u_int8_t*)abmp1 + sizeof(struct list_head));
	p2 = (u_int32_t*)((u_int8_t*)abmp2 + sizeof(struct list_head));

	count = 1 + agmp->agg_bitmap_len + abmp1->bmp_cnt;
	for(i= 0; i<count; i++){
		if(p1[i] != p2[i]){
			return -1;
                }
	}
	return 0;
}

/**
 * @brief compare two aggregated bitmap
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp1 aggregated bitmap1
 * @param[in] abmp2 aggregated bitmap2
 * @return  1  abmp1 >  abmp2
 *          -1 abmp1 <  abmp2
 *          0  abmp1 == abmp2
 */
s_int32_t abmp_cmp(struct agg_master * agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2)
{
	u_int32_t i,count, *p1, *p2;
	p1 = (u_int32_t*)(abmp1 + 1);
	p2 = (u_int32_t*)(abmp2 + 1);

	count = agmp->agg_bitmap_len + abmp1->bmp_cnt;
	for(i= 0; i<count; i++){
		if(p1[i] > p2[i]){
			return 1;
                }
                if(p1[i] < p2[i]){
                        return -1;
                }

	}
	return 0;
}

/**
 * @brief search abmp  in agg_table
 * @param[in] agg_table_master ptr to the agg_table 
 * @param[in] agtp ptr to the agg_table 
 * @param[in] abmp ptr to the agg_bitmap
 * return ~((u_int32_t)0)  not found
 * return idx in agtp
 */
u_int32_t search_in_table(struct agg_master *agmp, struct agg_table *agtp, struct agg_bitmap *abmp){
        struct agg_bitmap *abmp_tmp;
        struct list_head  *pos;
        u_int32_t i = 0;
        list_for_each(pos, &agtp->abmp_head){
            abmp_tmp = container_of(pos, struct agg_bitmap, abmp_link);
            if(!abmp_equal(agmp, abmp_tmp, abmp)){
                            return i;
            }
       }
       return ~((u_int32_t)0);
}

/**
 * @brief debug agg_bitmap
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp ptr to the agg_bitmap
 *
 */
void debug_agg_bitmap(struct agg_master *agmp, struct agg_bitmap *abmp){
    u_int32_t i;
    u_int32_t *p;
    fprintf(stderr, "agg_bitmap:");
    fprintf(stderr, "agg_bitmap->bmp_cnt: %u\n", abmp->bmp_cnt);

    p = (u_int32_t*)(abmp + 1);

    fprintf(stderr, "aggregated table:");
    for(i = 0; i < agmp->agg_bitmap_len; i++){
        fprintf(stderr, "%x ", *(p++));
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "bmp:");
    for(i = 0; i < agmp->bitmap_len, i < 10; i++){
        fprintf(stderr, "%x ", *(p++));
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "\n");
}

/**
 * @brief debug agg_table
 * @param[in] agmp ptr to the agg_master
 * @param[in] agtp ptr to the agg_table
 *
 */
void debug_agg_table(struct agg_master *agmp, struct agg_table *agtp){
        struct agg_bitmap *abmp_tmp;
        struct list_head  *pos;
        u_int32_t i = 0;
        fprintf(stderr, "agg_table:");
        fprintf(stderr, "agg_table->abmp_cnt: %u\n", agtp->abmp_cnt);
        fprintf(stderr, "abmp items:\n");
        list_for_each(pos, &agtp->abmp_head){
            fprintf(stderr, "item %u\n", i++);
            abmp_tmp = container_of(pos, struct agg_bitmap, abmp_link);
            debug_agg_bitmap(agmp, abmp_tmp);
        }
        fprintf(stderr, "\n");
}

/**
 * @brief debug agg_table
 * @param[in] agmp ptr to the agg_master
 * @param[in] agtp ptr to the agg_table
 *
 */
void debug_agg_master(struct agg_master *agmp){
        fprintf(stderr, "agg_master:");
        fprintf(stderr, "agg_master->table_cnt: %u\n",       agmp->table_cnt);
        fprintf(stderr, "agg_master->abmp_cnt: %u\n",        agmp->abmp_cnt);
        fprintf(stderr, "agg_master->mem_cnt: %u\n",         agmp->mem_cnt);

        fprintf(stderr, "agg_master->agg_bitmap_len: %u(32bit/step)\n",   agmp->agg_bitmap_len);
        fprintf(stderr, "agg_master->bitmap_len    : %u(32bit/step)\n",   agmp->bitmap_len);
        fprintf(stderr, "agg_master->bit_len       : %u( 1bit/step)\n",   agmp->bit_len);

        fprintf(stderr, "agg_bitmap in mem:\n");
        debug_agg_bitmap(agmp, (struct agg_bitmap *)agmp->mem);

        fprintf(stderr, "\n");
}
















