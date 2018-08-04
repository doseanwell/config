/**************************************************************
* 文件名:
*   option.h
*
*   描述:
*       option.c的头文件
*
* 版本：V1.0.0：由sean创建
***************************************************************/
#ifndef __OPTION__
#define __OPTION__

#include <stdio.h>
#include <stdint.h>
#include "list.h"
#define SYSTEM_VERSION  "1.0.0"
#define HEADER_COMMENT "ZHD MTXY SYSTEM CONFIG FILE"

#define FORMAT_INIT         0
#define FORMAT_DOUBLE       1
#define FORMAT_STRING       2
#define FORMAT_ENUM         3

#define myfree(p)   free(p); \
                    p = NULL
typedef struct {        /* option type */
    char *name;         /* option name */
    int format;         /* option format (0:int,1:double,2:string,3:enum) */
    void *var;          /* pointer to option variable */
    char *comment;      /* option comment/enum labels/unit */
} opt_type;

typedef struct optlq_type{        /* option link queue */
    opt_type opt;
    struct optlq_type *prev;
    struct optlq_type *next;
} opt_lq_type;

/*************option api**************/

int creat_opts(const char *file);
opt_lq_type *add_one_opts(opt_lq_type *opt_lq, char *name, int format, void *var, char *comment);
opt_lq_type *load_opts(const char *file, opt_lq_type *opt_lq);
int save_opt(const char *file, opt_lq_type *opt_lq, const char *comment);//该函数加锁设计

#endif //__common_h__
