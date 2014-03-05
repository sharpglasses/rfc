#include "aggregate_master.h"
#include "string.h"
#include "stdlib.h"

int main()
{
    struct  agg_master *master1;
    struct  agg_table *table1; 
    master1 = alloc_agg_master(malloc, free, 65535);
    table1  = alloc_agg_table(master1);
    debug_agg_master(master1);
    return 0;
}
