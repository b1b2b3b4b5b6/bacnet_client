/*
 * @Author: lumos
 * @Date: 2018-12-03 15:50:55
 * @LastEditors: lumos
 * @LastEditTime: 2018-12-03 17:37:10
 * @Description: file content
 * @Email: a1439458305@163.com
 */
#ifndef STD_COMMON_H
#define STD_COMMON_H

#include "errno.h"
#include "time.h"
#include "semaphore.h"
#include "string.h"
#include "stdlib.h"
#include "stdio.h"
#include "pthread.h"
#include "stdint.h"
#include "assert.h"
#include <sys/time.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>


#define STD_TIME 0
#define STD_GLOBAL_LOG_LEVEL STD_LOG_DEBUG
#define PRODUCT_TEST 1

#if STD_TIME 
    #define TIME_STR ({ \
        time_t s_now;\
        time(&s_now);\
        asctime(localtime(&s_now));\
    })
#else
    #define TIME_STR ""
#endif

#define _STD_LOGE(format, ...) do{\
    if(STD_LOCAL_LOG_LEVEL >= STD_LOG_ERROR){\
        printf( "\033[31m%s{%s | line %d ] : "format"\r\n\033[0m", TIME_STR , __FILE__, __LINE__,  ##__VA_ARGS__);\
    }\
}while(0)

#define _STD_LOGW(format, ...) do{\
    if(STD_LOCAL_LOG_LEVEL >= STD_LOG_WARN){\
        printf( "\033[33m%s[%s | line %d ] : "format"\r\n\033[0m", TIME_STR, __FILE__, __LINE__,  ##__VA_ARGS__ );\
    }\
}while(0)

#define _STD_LOGI(format, ...) do{\
    if(STD_LOCAL_LOG_LEVEL >= STD_LOG_INFO){\
        printf( "\033[32m%s{%s | line %d ] : "format"\r\n\033[0m", TIME_STR , __FILE__, __LINE__, ##__VA_ARGS__);\
    }\
}while(0)

#define _STD_LOGD(format, ...) do{\
    if(STD_LOCAL_LOG_LEVEL >= STD_LOG_DEBUG){\
        printf( "\033[0m%s{%s | line %d ] : "format"\r\n\033[0m", TIME_STR , __FILE__, __LINE__,  ##__VA_ARGS__);\
    }\
}while(0)

#define _STD_LOGV(format, ...) do{\
    if(STD_LOCAL_LOG_LEVEL >= STD_LOG_VERBOSE){\
        printf( "\033[34m%s{%s | line %d ] : "format"\r\n\033[0m", TIME_STR , __FILE__, __LINE__,  ##__VA_ARGS__);\
    }\
}while(0)

#define STD_LOGE(format, ...) do{\
    if(STD_GLOBAL_LOG_LEVEL >= STD_LOG_ERROR){\
        _STD_LOGE(format, ##__VA_ARGS__);\
    }\
}while(0) 

#define STD_LOGW(format, ...) do{\
    if(STD_GLOBAL_LOG_LEVEL >= STD_LOG_WARN){\
        _STD_LOGW(format, ##__VA_ARGS__);\
    }\
}while(0) 

#define STD_LOGI(format, ...) do{\
    if(STD_GLOBAL_LOG_LEVEL >= STD_LOG_INFO){\
        _STD_LOGI(format, ##__VA_ARGS__);\
    }\
}while(0) 

#define STD_LOGD(format, ...) do{\
    if(STD_GLOBAL_LOG_LEVEL >= STD_LOG_DEBUG){\
        _STD_LOGD(format, ##__VA_ARGS__);\
    }\
}while(0) 

#define STD_LOGV(format, ...) do{\
    if(STD_GLOBAL_LOG_LEVEL >= STD_LOG_VERBOSE){\
        _STD_LOGV(format, ##__VA_ARGS__);\
    }\
}while(0) 



                                                                                                                                       

#define STD_PARAM_CHECK(arg) if(!arg) printf("error\r\n")
#define STD_FAIL -1
#define STD_OK 0
#define STD_ERROR_GOTO_FAIL(con) do { \
        if (con != 0) { \
            STD_LOGE("error jump here"); \
            goto FAIL; \
        } \
    }while (0)


#ifndef MAC2STR
#define MAC2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5]
#define MACSTR "%02x:%02x:%02x:%02x:%02x:%02x"
#endif

typedef enum log_level_t{
    STD_LOG_ERROR = 1,
    STD_LOG_WARN,
    STD_LOG_INFO,
    STD_LOG_DEBUG,
    STD_LOG_VERBOSE,
}log_level_t;

#define STD_FREE(ptr) free(ptr)

#endif