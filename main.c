//#include "map.h"

#include "rfc_mem.h"

int main()
{
    struct rfc_mem *mem;
    mem = alloc_rfc_mem(malloc, free, malloc, free);
    if(!mem){
        fprintf(stderr, "alloc mem fail\n");
    }
    destory_rfc_mem(mem);
    return 0;
}
