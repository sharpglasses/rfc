/**
 * @brief rfc entry 
 * @file rfc_entry.c
 * @author mhw
 * @note 
 * @date 2014/3/13
 */
#include "rfc_entry.h"

u_int32_t count_bit(u_int32_t x){
    u_int32_t i = 0;
    while(x){
        x &= x-1;
        i++;
    }
    return i;
}



