/**
 * @brief rfc aggregate aggbitmap manager
 * @file aggregate_master.c
 * @author mhw
 * @date 2014/3/10
 */
#include "aggregate_master.h"

#include "stdlib.h" 

/**
 * @brief alloc agg_master
 * @param[in] agg_alloc ptr used to alloc mem
 * @param[in] agg_destory ptr used to free mem
 * @param[in] bit_len tot bits of bitmap wait to aggregate
 * return ptr to the agg_master
 */
struct agg_master *alloc_agmp(void *(*agg_alloc)(u_int32_t), void (*agg_destory)(void *), u_int32_t bit_len){
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
 * @brief destory_agmp
 * @param[in] agmp ptr to the agg_master
 * @return 
 */
void destory_agmp(struct agg_master *agmp){
        struct agg_table *agg_table_ptr;
        while(agmp->table_cnt){
                agg_table_ptr = container_of(agmp->table_head.next, struct agg_table, table_link);
                destory_agtp(agmp, agg_table_ptr);
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
struct  agg_table * alloc_agtp(struct agg_master *agmp){
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
    return new_agg_table;
};

/**
 * @brief destory_agtp
 * @param[in] agtp ptr to the agg_master
 * @return 
 */
void destory_agtp(struct agg_master *agmp, struct agg_table *agtp)
{
    u_int32_t i;
    struct agg_bitmap *abmp_tmp;
    while(agtp->abmp_cnt){
            abmp_tmp = container_of(agtp->abmp_head.next, struct agg_bitmap, abmp_link);
            remove_abmp(agtp, abmp_tmp);
            destory_abmp(agmp, abmp_tmp);
    }
    list_del(&agtp->table_link);
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
struct agg_bitmap *alloc_abmp(struct agg_master *agmp){
    struct agg_bitmap *abmp_tmp;
    u_int32_t x;
    abmp_tmp = (struct agg_bitmap*)agmp->mem;
    x = ABMP_HEAD_SIZE + 4*abmp_tmp->bmp_cnt + 4*agmp->agg_bitmap_len;
    abmp_tmp = (struct agg_bitmap *)agmp->agg_alloc(x);
    if(!abmp_tmp){
        return 0;
    }
    memcpy(abmp_tmp, agmp->mem, x);
    agmp->mem_cnt += x;
    agmp->abmp_cnt++;
    return abmp_tmp;
}

/**
 * @brief alloc space for aggregate
 * @param[in] agmp ptr to agg_master
 * @param[in] abmp ptr to normal bitmap
 * @return 
 */
void destory_abmp(struct agg_master *agmp, struct agg_bitmap * abmp){
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
void add_abmp(struct agg_table *agtp, struct agg_bitmap *abmp){
    list_add(&abmp->abmp_link, &agtp->abmp_head);
    agtp->abmp_cnt++;
};

/**
 * @brief add  agg_bitmap into agg_table tail
 * @param[in]  agtp  the table want to save this aggregated bitmap
 * @param[in]  abmp the ptr to the aggregated bitmap
 */
void add_abmp_tail(struct agg_table *agtp, struct agg_bitmap *abmp){
    list_add_tail(&abmp->abmp_link, &agtp->abmp_head);
    agtp->abmp_cnt++;
};

/**
 * @brief remove agg_bitmap from the agg_table 
 * @param[in]  agtp  the aggregated bitmap in 
 * @param[in]  abmp the ptr to the aggregated bitmap
 */
void remove_abmp(struct agg_table *agtp, struct agg_bitmap *abmp){
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

	for(i=0; i< agmp->bitmap_len; i++)
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
 * @brief get aggregated bitmap by aggregated bitmap1 & aggregated bitmap2
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp1 aggregated bitmap1
 * @param[in] abmp2 aggregated bitmap2
 * @return aggregated bitmap
 */
struct agg_bitmap *abmp_and(struct agg_master *agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2)
{
	u_int32_t i,j;
	u_int32_t *p12,*p13;
	u_int32_t *p22,*p23;
	struct agg_bitmap *abmp;
	u_int32_t *p2,*p3;
	u_int32_t mask;
	u_int32_t detail_bmp_count = 0;

        memset(agmp->mem, 0, ABMP_HEAD_SIZE + 4*agmp->agg_bitmap_len + 4*agmp->bitmap_len);
	abmp = (struct agg_bitmap *)agmp->mem;

	p12 = (u_int32_t*)(abmp1 + 1);
	p13 = p12 + agmp->agg_bitmap_len;
	p22 = (u_int32_t*)(abmp2 + 1);
	p23 = p22 + agmp->agg_bitmap_len;

	p2 = (u_int32_t*)(abmp + 1);
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
	abmp->bmp_cnt = detail_bmp_count;
	return abmp;
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
	struct agg_bitmap *abmp;
	u_int32_t *p2,*p3;
	u_int32_t mask;
	u_int32_t detail_bmp_count = 0;

        memset(agmp->mem, 0, ABMP_HEAD_SIZE + 4*agmp->agg_bitmap_len + 4*agmp->bitmap_len);
	abmp = (struct agg_bitmap *)agmp->mem;

	p12 = (u_int32_t*)(abmp1 + 1);
	p13 = p12 + agmp->agg_bitmap_len;
	p22 = (u_int32_t*)(abmp2 + 1);
	p23 = p22 + agmp->agg_bitmap_len;

	p2 = (u_int32_t*)(abmp + 1);
	p3 = p2 + agmp->agg_bitmap_len;

	for(i=0; i< agmp->agg_bitmap_len; i++)
	{
		mask = 0x1;
		for(j=0;j<32;j++)
		{
			if(mask & *p12){
				*p3 |= *p13;
				p13++;
			}
			if(mask & *p22){
				*p3 |= *p23;
				p23++;
                        }
			if((mask & (*p12)) || (mask & (*p22)))
			{
				*p2 |= mask;
				detail_bmp_count++;
				p3++;
			}
			mask <<= 1;
		}
		p12++;
		p22++;

	}
	abmp->bmp_cnt = detail_bmp_count;
	return abmp;
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
s_int32_t abmp_cmp(struct agg_master *agmp, struct agg_bitmap *abmp1, struct agg_bitmap *abmp2)
{
	u_int32_t i,j;
	u_int32_t *p12,*p13;
	u_int32_t *p22,*p23;
	u_int32_t mask;

	p12 = (u_int32_t*)(abmp1 + 1);
	p13 = p12 + agmp->agg_bitmap_len;

	p22 = (u_int32_t*)(abmp2 + 1);
	p23 = p22 + agmp->agg_bitmap_len;

	for(i=0; i< agmp->agg_bitmap_len; i++)
	{
	        if(*p12 != *p22){
	            return (*p12 > *p22 ? 1 : -1);
	        }
		mask = 0x1;
		for(j=0;j<32;j++)
		{
		    if(mask & (*p12) & (*p22)){
                        if(*p13 != *p23){
                            return (*p13 > *p23 ? 1 : -1);
                        }
		        p13++;
		        p23++;
		    }
		    mask<<= 1;
		}
		p12++;
		p22++;

	}
	return 0;
}

/**
 * @brief search abmp  in agg_table
 * @param[in] agmp ptr to the agg_table 
 * @param[in] agtp ptr to the agg_table 
 * @param[in] abmp ptr to the agg_bitmap
 * return ~((u_int32_t)0)  not found
 * return idx in agtp
 */
u_int32_t search_abmp_in_table(struct agg_master *agmp, struct agg_table *agtp, struct agg_bitmap *abmp){
        struct agg_bitmap *abmp_tmp;
        struct list_head  *pos;
        u_int32_t i = 0;
        list_for_each(pos, &agtp->abmp_head){
            abmp_tmp = container_of(pos, struct agg_bitmap, abmp_link);
            if(!abmp_equal(agmp, abmp_tmp, abmp)){
                            return i;
            }
            i++;
       }
       return ~((u_int32_t)0);
}


/**
 * @brief sort all item in agtp
 * @param[in] agmp ptr to agg_master 
 * @param[in] agtp ptr to the agg_table 
 * return idx in agtp
 * TODO BUGS IN insert_sort_agtp
 */
void insert_sort_agtp(struct agg_master *agmp, struct agg_table *agtp){
    struct agg_bitmap *abmp_head;
    struct agg_bitmap *abmp;
    struct agg_bitmap *abmp_tmp;
    struct list_head  *pos;
    if(!agtp->abmp_cnt){
        return;
    }
    agtp->abmp_cnt = 1;
    abmp_head = container_of(agtp->abmp_head.next, struct agg_bitmap, abmp_link);
    remove_abmp(agtp, abmp_head);
    while(!list_empty(&agtp->abmp_head)){
            abmp = container_of(agtp->abmp_head.next, struct agg_bitmap, abmp_link);
            remove_abmp(agtp, abmp);

            if(abmp_cmp(agmp, abmp, abmp_head) < 0){
                    list_add_tail(&abmp->abmp_link, &abmp_head->abmp_link);
                    abmp_head = abmp;               
            }else{
                    list_for_each(pos, &abmp_head->abmp_link){
                        abmp_tmp = container_of(pos, struct agg_bitmap, abmp_link);
                        if(abmp_cmp(agmp, abmp_tmp, abmp) > 0){
                            break;
                        }
                    }
                    if(pos == &abmp_head->abmp_link){  
                            list_add_tail(&abmp->abmp_link,  &abmp_head->abmp_link);
                            abmp_head = abmp;
                    }else{                  
                            list_add_tail(&abmp->abmp_link, &abmp_tmp->abmp_link);
                    }
            }
        agtp->abmp_cnt++;   
    }
    list_add_tail(&agtp->abmp_head, &abmp_head->abmp_link);
}



/**
 * @brief debug agg_bitmap
 * @param[in] agmp ptr to the agg_master
 * @param[in] abmp ptr to the agg_bitmap
 *
 */
void debug_abmp(struct agg_master *agmp, struct agg_bitmap *abmp){
    u_int32_t i;
    u_int32_t *p;
    fprintf(stderr, "agg_bitmap:");
    fprintf(stderr, "agg_bitmap->bmp_cnt: %u\n", abmp->bmp_cnt);

    p = (u_int32_t*)(abmp + 1);

    fprintf(stderr, "aggreated bmp:");
    for(i = 0; i < agmp->agg_bitmap_len; i++){
        fprintf(stderr, "%x ", *(p++));
    }
    fprintf(stderr, "\n");

    fprintf(stderr, "bmp:");
    for(i = 0; i < abmp->bmp_cnt && i < 32; i++){
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
void debug_agtp(struct agg_master *agmp, struct agg_table *agtp){
        struct agg_bitmap *abmp_tmp;
        struct list_head  *pos;
        u_int32_t i = 0;
        fprintf(stderr, "agg_table:\n");
        fprintf(stderr, "agg_table->abmp_cnt: %u\n", agtp->abmp_cnt);
        fprintf(stderr, "abmp items:\n");
        list_for_each(pos, &agtp->abmp_head){
            fprintf(stderr, "item %u\n", i++);
            abmp_tmp = container_of(pos, struct agg_bitmap, abmp_link);
            debug_abmp(agmp, abmp_tmp);
        }
        fprintf(stderr, "\n");
}

/**
 * @brief debug agg_table
 * @param[in] agmp ptr to the agg_master
 * @param[in] agtp ptr to the agg_table
 *
 */
void debug_agmp(struct agg_master *agmp){
        fprintf(stderr, "agg_master:\n");
        fprintf(stderr, "agg_master->table_cnt: %u\n",       agmp->table_cnt);
        fprintf(stderr, "agg_master->abmp_cnt: %u\n",        agmp->abmp_cnt);
        fprintf(stderr, "agg_master->mem_cnt: %u\n",         agmp->mem_cnt);

        fprintf(stderr, "agg_master->agg_bitmap_len: %u(32bit/step)\n",   agmp->agg_bitmap_len);
        fprintf(stderr, "agg_master->bitmap_len    : %u(32bit/step)\n",   agmp->bitmap_len);
        fprintf(stderr, "agg_master->bit_len       : %u( 1bit/step)\n",   agmp->bit_len);

        fprintf(stderr, "agg_bitmap in mem:\n");
        debug_abmp(agmp, (struct agg_bitmap *)agmp->mem);

        fprintf(stderr, "\n");
}

/* code below used for test
 *
 *
 */
/*
void debug_bmp(u_int32_t *bmp, u_int32_t start, u_int32_t end)
{
    u_int32_t i;
    fprintf(stderr, "bmp:");
    for(i = start; i < end; i++){
            fprintf(stderr, "%x ", bmp[i]);   
    }
    fprintf(stderr, "\n");
    fprintf(stderr, "\n");
}


int main ()
{
    int i;
    u_int32_t bit_len = 32000;
    struct agg_master *agmp;
    struct agg_table  *agtp, *agtp1;
    struct agg_bitmap *abmp, *abmp1, *abmp2;

    u_int32_t *bmp = (u_int32_t *)malloc(bit_len/8);
    bmp[1] = 0x01;
    bmp[2] = 0x02;
    bmp[3] = 0x03;
    bmp[33] = 0x15;
    debug_bmp(bmp, 0, 3);
    debug_bmp(bmp, 33, 35);
    agmp = alloc_agmp(malloc, free, bit_len);
    if(!agmp){
        fprintf(stderr, "alloc agg master fail\n");
        return -1;
    }
    agtp = alloc_agtp(agmp);
    if(!agtp){
        fprintf(stderr, "alloc agg table fail\n");
        return -1;
    }

    agtp1 = alloc_agtp(agmp);
    if(!agtp1){
        fprintf(stderr, "alloc agg table1 fail\n");
        return -1;
    }
    
    fprintf(stderr, "convert bmp to abmp\n");
    abmp = bmp_to_abmp(agmp, bmp);
    
    fprintf(stderr, "save abmp\n");
    abmp = alloc_abmp(agmp);
    if(!abmp){
        fprintf(stderr, "alloc agg bitmap fail\n");
        return -1;
    }
    debug_abmp(agmp, abmp);

    fprintf(stderr, "add abmp to table\n");
    add_abmp(agtp, abmp);
    debug_agtp(agmp, agtp);
    
    fprintf(stderr, "set bmp1\n");
    bmp[2] = 0x2; 
    fprintf(stderr, "convert bmp to abmp\n");
    abmp1 = bmp_to_abmp(agmp, bmp);

    fprintf(stderr, "save abmp1\n");
    abmp1 = alloc_abmp(agmp);
    if(!abmp1){
        fprintf(stderr, "alloc agg bitmap1 fail\n");
        return -1;
    }

    fprintf(stderr, "add abmp1 to table\n");
    add_abmp(agtp1, abmp1);
    debug_agtp(agmp, agtp1);

    fprintf(stderr, "and bitmap, save abmp to table\n");
    abmp1 =  abmp_and(agmp, abmp, abmp1);
    abmp1 = alloc_abmp(agmp);
    add_abmp(agtp, abmp1);
    debug_agtp(agmp, agtp);

    i = abmp_equal(agmp, abmp1, agmp->mem);
    fprintf(stderr, "get %d from equal\n", i);

    i = abmp_equal(agmp, abmp1, abmp);
    fprintf(stderr, "get %d from equal\n", i);

    i =  search_abmp_in_table(agmp, agtp, abmp);
    fprintf(stderr, "get idx %u from search\n", i);

    i =  search_abmp_in_table(agmp, agtp, abmp1);
    fprintf(stderr, "get idx %u from search\n", i);

    bmp[2] = 0x1; 
    bmp[33] = 0xff;
    abmp2 = bmp_to_abmp(agmp, bmp);
    abmp2 = alloc_abmp(agmp);

    i =  search_abmp_in_table(agmp, agtp, abmp2);
    fprintf(stderr, "get idx %u from search\n", i);
    add_abmp(agtp, abmp2);

    fprintf(stderr, "insert sort table\n"); 
    insert_sort_agtp(agmp, agtp);
    debug_agtp(agmp, agtp);

    i =  search_abmp_in_table(agmp, agtp, abmp2);
    fprintf(stderr, "get idx %u from search\n", i);
    debug_agtp(agmp, agtp);
    remove_abmp(agtp, abmp1);
    fprintf(stderr, "move bmp from agtp to agtp1\n");
    add_abmp(agtp1, abmp1);

    debug_agtp(agmp, agtp);
    debug_agtp(agmp, agtp1);

    i = abmp_cmp(agmp, abmp, abmp1);
    fprintf(stderr, "get %d from cmp\n", i);

    i = abmp_cmp(agmp, abmp1, abmp);
    fprintf(stderr, "get %d from cmp\n", i);

    i = abmp_cmp(agmp, abmp, abmp);
    fprintf(stderr, "get %d from cmp\n", i);

    i =  search_abmp_in_table(agmp, agtp, abmp);
    fprintf(stderr, "get idx %u from search\n", i);

    debug_agmp(agmp);

    abmp1 = abmp_or(agmp, abmp1, abmp2);
    fprintf(stderr, "abmp or \n");
    debug_abmp(agmp, abmp1);

    fprintf(stderr, "destory abmp table\n");
    destory_agtp(agmp, agtp);
    debug_agmp(agmp);
    debug_agtp(agmp, agtp1);
    destory_agmp(agmp);
    //memset(bmp, 0, bit_len/8);
    //abmp_to_bmp(agmp, abmp, bmp);
    //debug_bmp(bmp, 0, 4);
    //debug_bmp(bmp, 33, 35);

    return 0;
}



*/
















