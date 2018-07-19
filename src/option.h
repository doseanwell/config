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
#include "persist_common.h"

#define SYSTEM_VERSION  "1.0.0"
#define HEADER_COMMENT "ZHD MTXY SYSTEM CONFIG FILE"

#define FORMAT_INIT         0
#define FORMAT_DOUBLE       1
#define FORMAT_STRING       2
#define FORMAT_ENUM         3

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

typedef struct {        /* option login type */
    char login_ip[40];
    int  login_port;
} opt_login;

typedef struct {        /* option cors type */
    int port;           /* cors 服务端口号 */
    int type;           /* cors 查分类型 0:内置网络　1:手簿差分*/
    int mode;          /* cors模式类型 0-vrs 1-zhd 2-ppp 3-qxwz */

    char vrs_ip[40];
    int  vrs_port;
    char vrs_username[20];  /* 用户名 */
    char vrs_password[20];  /* 用户密码 */
    char vrs_datatype[20];  /* 差分数据类型 */

    char zhd_ip[40];
    int  zhd_port;
    int  zhd_usergroup;  /* 用户组 */
    int  zhd_workgroup;  /* 工作组 */

    char qxwz_ip[40];
    int  qxwz_port;
    char qxwz_username[20];  /* 用户名 */
    char qxwz_password[20];  /* 用户密码 */
    char qxwz_datatype[20];  /* 差分数据类型 */
} opt_cors;
/************opt_sta枚举类型*************/
typedef enum {
    OPT_STA_FLAG          = 0,
    OPT_STA_LED,
    OPT_STA_KEY,
    OPT_STA_AUDIO,
    OPT_STA_UPGRADE,
    OPT_STA_REG,
    OPT_STA_SERVER,
    OPT_STA_LOGIN,
    OPT_STA_POSITION,
    OPT_STA_GNSS_ROVER,
    OPT_STA_GNSS_STATIC,
    OPT_STA_DETECT,
    OPT_STA_WARN,
    OPT_STA_CONNECT,
    OPT_STA_TEST,
}ENUM_OPT_STA_TYPE;
/************opt_sta枚举类型*************/
typedef struct {        /*  */
    uint32_t flag;           /*  */
    uint32_t led;           /*  */
    uint32_t key;          /*  */
    uint32_t audio;
    uint32_t upgrade;
    uint32_t reg;  /*  */
    uint32_t server;  /*  */
    uint32_t login;  /*  */
    uint32_t position;
    uint32_t gnss_rover;
    uint32_t gnss_static;  /*  */
    uint32_t detect;  /* 工作组 */
    uint32_t misc;/* 0:空闲未有工作;0x01:gga采集中 按位表示，第一位表示;0x02:网络测试记录 第二位表示 */
    uint32_t warn;
    uint32_t connect;
    uint32_t test;  /*  */
} opt_sta;
/************opt_sta枚举类型*************/
typedef enum {
    OPT_SYS_RESET          = OPT_STA_TEST + 1,//接着OPT_STA_TYPE
    OPT_SYS_VERSION,
    OPT_SYS_CONF_VER,
    OPT_SYS_DEV_NUM,
    OPT_SYS_REG_NUM,
    OPT_SYS_REG_TIME,
    OPT_SYS_REG_FLAG,
    OPT_SYS_DEV_TYPE,
    OPT_SYS_PRODUCT_NAME,
    OPT_SYS_COMPANY_NAME,
    OPT_SYS_PRODUCT_TYPE,
    OPT_SYS_PRODUCT_TYPE_SUB,
    OPT_SYS_WORK_MODE,
    OPT_SYS_GNSSBOARD_VERSION,
    OPT_SYS_GNSSBOARD_TYPE,
    OPT_SYS_GNSSBOARD_RESET,
    OPT_SYS_ANT_TYPE,
    OPT_SYS_NETMODULE_TYPE,
    OPT_SYS_BT_TYPE,
    OPT_SYS_BT_RESET,
    OPT_SYS_CONNECT_TYPE,
}ENUM_OPT_SYS_TYPE;
/************opt_sta枚举类型*************/
typedef struct {            /* option core type */
    uint32_t reset;              /* 系统复位　*/
    uint8_t version[12];       /* 系统版本 */
    uint8_t conf_ver[12];      /* 系统版本 */
    uint8_t dev_num[20];       /* 设备机号 */
    uint32_t dev_num_c;       /* 设备机号校验码 */
    uint8_t reg_num[32];       /* 24位注册码 */
    uint8_t reg_time[20];      /* 注册码注册到期时间 */
    uint8_t last_update_time[40];/* 上次升级时间 */
    uint32_t reg_flag;          /* 注册码有效标志 */
    uint32_t dev_type;           /* 0:人员设备 1:机车设备 */
    uint8_t product_name[12];
    uint8_t company_name[12];
    uint32_t product_type;       /*主型号*/
    uint32_t product_type_sub;   /*次型号*/
    uint32_t work_mode;
    uint32_t work_run;           /*工作模式运行状态*/
    uint8_t gnssboard_version[20];
    uint32_t gnssboard_type;
    uint32_t gnssboard_reset;
    uint32_t ant_type;
    uint32_t netmodule_type;
    uint32_t bt_type;
    uint32_t bt_reset;
    uint32_t connect_type;

//  uint32_t dayofyear;
//  uint32_t time_part;//时段位（为静态文件自动命名服务）,取值0~9和a~z
//  uint32_t file_ext;//扩展符（为静态文件自动命名服务）,取值_和a~z

    uint32_t gga_record;//gga记录使能 0 不记录  1 记录
    uint32_t volume;//系统音量1~10
} opt_sys;

typedef struct {
    pthread_mutex_t lock; /* 互斥体lock 用于对缓冲区的互斥操作 */
    pthread_cond_t not_true; /* 缓冲区非空的条件变量 */
//  opt_login   login_opt;
    opt_sta     sta;
    opt_sys     sys;
} opt_info_type;

/*************option api**************/

int creat_opts(const char *file);
opt_lq_type *add_one_opts(opt_lq_type *opt_lq, char *name, int format, void *var, char *comment);
opt_lq_type *load_opts(const char *file, opt_lq_type *opt_lq);
int save_opt(const char *file, opt_lq_type *opt_lq, const char *comment);//该函数加锁设计

/*****下面为system专用函数******/
opt_info_type *system_conf_init(const char *file);
int save_system_opt();
//opt_info_type *get_sysopts();
void opts_write(uint32_t opt_type, void *val);
/*************option api**************/

#endif //__common_h__
