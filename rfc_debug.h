#ifndef  RFc_DEBUG_H_
#define  RFc_DEBUG_H_

#include <errno.h>
#include <stdio.h>

#include "rfc_conf.h"

/**
 * @brief 存放rfc调试接口
 * @file rfc_debug.h
 * @author mhw
 * @date 2014/3/3
 *
 */

enum {
    RFc_PRIVILEGE_FATAL = 0,
    RFc_PRIVILEGE_ERROR,
    RFc_PRIVILEGE_WARN,
    RFc_PRIVILEGE_NOTICE,
    RFc_PRIVILEGE_INFO,
    RFc_PRIVILEGE_DEBUG,
    RFc_PRIVILEGE_MAX
};


extern s_int32_t rfc_privilege;
extern void set_debug_privilege(s_int32_t privilege);


#define rfc_fatal(format, args...)\
            do{\
                if(RFc_PRIVILEGE_FATAL <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
                }\
            }while(0)


#define rfc_error(format, args...)\
            do{\
                if(RFc_PRIVILEGE_ERROR <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
                }\
	    }while(0)


#define rfc_warn(format, args...)\
            do{\
                if(RFc_PRIVILEGE_WARN <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
		}\
	    }while(0)


#define rfc_notice(format, args...)\
            do{\
                if(RFc_PRIVILEGE_NOTICE <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
		}\
	    }while(0)


#define rfc_info(format, args...)\
            do{\
                if(RFc_PRIVILEGE_INFO <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
		}\
	    }while(0)


#define rfc_debug(format, args...)\
            do{\
                if(RFc_PRIVILEGE_DEBUG <= rfc_privilege){\
                    fprintf(stderr,"dbg<%s|%d>"format, __func__, __LINE__, ##args);   \
		}\
	    }while(0)


#endif
