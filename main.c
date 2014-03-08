#include "rfc.h"

#define CORE_CNT 1
#define RULE_MAX 65535
#define RULE_CNT 10
#define CON_SIZE  9
#define VAL_SIZE 20
#define BYT_SIZE 256


int main()
{
    u_int32_t i, j, k;

    u_int8_t *param1;
    u_int8_t *param2;
    u_int32_t val;

    struct rfc *rfcp;
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
    return 0;
}
