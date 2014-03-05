#include "rfc_debug.h"

/*调试级别*/
s_int32_t rfc_privilege;



/**
 * @brief 设置调试级别
 * @param[in] privilege 调试级别参考rfc_conf.h
 *
 */

void set_debug_privilege(s_int32_t privilege){
        rfc_privilege = privilege;
}
