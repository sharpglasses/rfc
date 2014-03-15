#include "rfc.h"

#define CORE_CNT 1
#define RULE_MAX 65535
#define RULE_CNT 20
#define CON_SIZE  8
#define VAL_SIZE 20
#define BYT_SIZE 256


#define PARSE_IDX 1

s_int32_t parse_callback(struct rfc *rfcp, struct parse_info *pinfo, void* input, void *output)
{
    debug_parse(pinfo);
return 0;
}


int main()
{
    u_int32_t i, j, k;

    u_int8_t *param1;
    u_int8_t *param2;
    u_int32_t val;

    struct rfc *rfcp;
    struct agg_master *agmp;            
    rfcp  = alloc_rfc(RULE_MAX, CON_SIZE, CORE_CNT, malloc, free, malloc, free);             /*alloc rfc*/
    if(!rfcp){              
        fprintf(stderr, "create rfc fail\n");
        return -1;
    }

    srand(time(NULL));
    for(i = 0; i < RULE_CNT; i++){
                param1 = (u_int8_t *)malloc(sizeof(u_int8_t)*CON_SIZE);
                if(!param1){
                    fprintf(stderr, "set condition1 code fail\n");
                    return -1;
                }
                param2 = (u_int8_t *)malloc(sizeof(u_int8_t)*CON_SIZE);
                if(!param2){
                    fprintf(stderr, "set condition1 code fail\n");
                    return -1;
                }
                for(j = 0; j < CON_SIZE; j++){
                    param1[j] = rand()%BYT_SIZE;

                }
                for(j = 0; j < CON_SIZE; j++){
                    param2[j] = rand()%BYT_SIZE;
                }
                val = rand()%VAL_SIZE;
                fprintf(stderr, "read rule: eqid %u, val %u\n", i, val);
                debug_con(rfcp, param1);
                debug_con(rfcp, param2);
                if(rfc_read_rule(rfcp, (void *)param1, (void *)param2, val) < 0){
                    fprintf(stderr, "read rule into rfcp fail\n");
                    return -1;
            }
    }
    fprintf(stderr, "read all rule success\n\n");
    debug_rfc(rfcp);  
    agmp = alloc_agmp(malloc, free, rfcp->rule_cnt);
    if(!agmp){
        fprintf(stderr, "alloc agmp fail\n\n");
        return -1;
    }
    /*TODO 测试parse 接口正确性
    param1 = (u_int8_t *)malloc(sizeof(u_int8_t)*CON_SIZE);
    param2 = (u_int8_t *)malloc(sizeof(u_int8_t)*CON_SIZE);
    param1[0] = 1;
    param1[1] = 1;
    param1[2] = 1;
    param1[3] = 1;
    param1[4] = 1;
    param1[5] = 1;
    param1[6] = 1;
    param1[7] = 1;


    param2[0] = 1;
    param2[1] = 1;
    param2[2] = 9;
    param2[3] = 3;
    param2[4] = 1;
    param2[5] = 1;
    param2[6] = 1;
    param2[7] = 1;
    if(rfc_read_rule(rfcp, (void *)param1, (void *)param2, val) < 0){
           fprintf(stderr, "read rule into rfcp fail\n");
            return -1;
    }
    debug_rfc(rfcp);  
    piece_parse_rule(PARSE_IDX, rfcp, rfcp->agmp, 0, 0, parse_callback);
    */
    if(rfc_build_prepare(rfcp, agmp, 0, rfcp->piece_cnt) < 0){
        fprintf(stderr, "prepare fail\n");
        return -1;
    }
    rfcp->tdestory(rfcp->rule);
    return 0;
}
