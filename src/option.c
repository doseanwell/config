/**************************************************************
* 文件名:
*   option.c
*
*   描述:
*       实现系统配置文件system.conf的读写操作
*       只需要关注load_opts和saveopts这两个函数即可
*
* 版本：V1.0.0：由sean创建
***************************************************************/
#include "option.h"
#include "shm.h"


opt_lq_type opt_lq_info_base, *opt_lq_info_node;

/* discard space characters at tail ------------------------------------------*/
static void chop(char *str)
{
    char *p;
    if ((p=strchr(str,'#'))) *p='\0'; /* comment */
    for (p=str+strlen(str)-1;p>=str&&!isgraph((int)*p);p--) *p='\0';
}
/* enum to string ------------------------------------------------------------*/
static int enum_2_str(char *s, const char *comment, int val)
{
    char str[32],*p,*q;
    int n;

    n=sprintf(str,"%d:",val);
    if (!(p=strstr(comment,str))) {
        return sprintf(s,"%d",val);
    }
    if (!(q=strchr(p+n,','))&&!(q=strchr(p+n,')'))) {
        strcpy(s,p+n);
        return strlen(p+n);
    }
    strncpy(s,p+n,q-p-n); s[q-p-n]='\0';
    return q-p-n;
}
/* search option ---------------------------------------------------------------
* search option record
* args   : char   *name     I  option name
*          opt_t  *opts     I  options table
*                              (terminated with table[i].name="")
* return : option record (NULL: not found)
*-----------------------------------------------------------------------------*/
extern opt_type *search_opt(const char *name, const opt_lq_type *opt_lq)
{
    opt_lq_type *node = opt_lq;
    int i;

//  printf("searchopt: name=%s\n",name);
    while (node) {
        if (strstr(node->opt.name, name)) {
            return &node->opt;
        }
        node = node->next;
    }
//  for (i = 0; *opts[i].name; i++) {
//      if (strstr(opts[i].name,name))
//      {
//          return (opt_type *)(opts+i);
//      }
//  }
    return NULL;
}
/* string to enum ------------------------------------------------------------*/
static int str_2_enum(const char *str, const char *comment, int *val)
{
    const char *p;
    char s[32];

    for (p=comment;;p++) {
       if (!(p=strstr(p,str))) break;
       if (*(p-1)!=':') continue;
       for (p-=2;'0'<=*p&&*p<='9';p--) ;
       return sscanf(p+1,"%d",val)==1;
    }
    sprintf(s,"%30.30s:",str);
    if ((p=strstr(comment,s))) { /* number */
        return sscanf(p,"%d",val)==1;
    }
    return 0;
}
/* string to option value ------------------------------------------------------
* convert string to option value
* args   : opt_t  *opt      O  option
*          char   *str      I  option value string
* return : status (1:ok,0:error)
*-----------------------------------------------------------------------------*/
extern int str_2_opt(opt_type *opt, const char *str)
{
    switch (opt->format) {
        case 0: *(int    *)opt->var=atoi(str); break;
        case 1: *(double *)opt->var=atof(str); break;
        case 2: strcpy((char *)opt->var,str);  break;
        case 3: return str_2_enum(str,opt->comment,(int *)opt->var);
        default: return 0;
    }
    return 1;
}
/* option value to string ------------------------------------------------------
* convert option value to string
* args   : opt_t  *opt      I  option
*          char   *str      O  option value string
* return : length of output string
*-----------------------------------------------------------------------------*/
extern int opt_2_str(const opt_type *opt, char *str)
{
    char *p=str;

    switch (opt->format) {
        case 0: p+=sprintf(p,"%d"   ,*(int   *)opt->var); break;
        case 1: p+=sprintf(p,"%.15g",*(double*)opt->var); break;
        case 2: p+=sprintf(p,"%s"   , (char  *)opt->var); break;
        case 3: p+=enum_2_str(p,opt->comment,*(int *)opt->var); break;
    }
    return (int)(p-str);
}
/* option to string -------------------------------------------------------------
* convert option to string (keyword=value # comment)
* args   : opt_t  *opt      I  option
*          char   *buff     O  option string
* return : length of output string
*-----------------------------------------------------------------------------*/
extern int opt_2_buf(const opt_type *opt, char *buff)
{
    char *p=buff;
    int n;

    p+=sprintf(p,"%-18s =",opt->name);
    p+=opt_2_str(opt,p);
    if (*opt->comment) {
        if ((n=(int)(buff+30-p))>0) p+=sprintf(p,"%*s",n,"");
        p+=sprintf(p," # (%s)",opt->comment);
    }
    return (int)(p-buff);
}
/*******************************************************************************
 * 名称: add_one_opts
 * 功能: 向指定配置文件中添加一条记录
 * 形参: file：要添加到的配置文件目标文件名及其路径
 * 返回: opt_info_type类型指针
 * 说明: 注意这个函数只是把配置信息构建成链表
 ******************************************************************************/
opt_lq_type *add_one_opts(opt_lq_type *opt_lq, char *name, int format, void *var, char *comment)
{
    opt_lq_type *node = NULL;

    opt_lq->opt.name = name;
    opt_lq->opt.format = format;
    opt_lq->opt.var = var;
    opt_lq->opt.comment = comment;

    opt_lq->next = (opt_lq_type *)calloc(1, sizeof(opt_lq_type));
    node = opt_lq->next;
    node->prev = opt_lq;
    node->next = NULL;
    return node;
}
/*******************************************************************************
 * 名称: delect_one_opts
 * 功能: 删除系统配置中的一条记录
 * 形参: file：文件名及其路径
 * 返回: opt_info_type类型指针
 * 说明: 暂时用不到
 ******************************************************************************/
opt_info_type *delect_one_opts(const char *file)
{
}
/*******************************************************************************
 * 名称: creat_opts
 * 功能: 创建系统配置文件
 * 形参: file：文件名及其路径
 *      num：创建的记录总条数
 * 返回: opt_type类型指针
 * 说明: 该函数用于创建配置文件，不限路径和文件名
 ******************************************************************************/
int creat_opts(const char *file)
{
    FILE *fp;
    char buff[2048];
    if (!access(file, 0)) {
        DEBUG_INFO("The %s file exist\n", file);
        if (!(fp = fopen(file,"r"))) {
                DEBUG_ERROR("%s is none\n",file);
                return -1;
        }
        if (fgets(buff,sizeof(buff),fp)) {//做个保护,说明有数据，直接返回
            fclose(fp);
            return 0;
        }
    }

    if (!(fp = fopen(file,"w+"))) {
        DEBUG_ERROR("%s is none\n",file);
        return -1;
    }

    DEBUG_INFO("%s creat success!\n",file);

    fclose(fp);
    sync();//确保及时写入磁盘
    return 0;

}
opt_lq_type *load_opts(const char *file, opt_lq_type *opt_lq)
{
    opt_lq_type *start = opt_lq, *node;

    FILE *fp;
    opt_type *opt;
    char buff[2048],*p;
    int n=0;

    if(creat_opts(file) < 0) {//先确保文件存在
        DEBUG_ERROR("%s is error\n",file);
        return NULL;
    }
    node = start;

    if (!(fp=fopen(file,"r"))) {
        DEBUG_ERROR("%s is none\n",file);
        return NULL;
    }

    while (fgets(buff,sizeof(buff),fp)) {
//      DEBUG_INFO("buff=%s\n",buff);
        n++;
        chop(buff);

        if (buff[0]=='\0') continue;

        if (!(p=strstr(buff,"="))) {
            fprintf(stderr,"invalid option %s (%s:%d)\n",buff,file,n);
            continue;
        }
        *p++='\0';
        chop(buff);
//      DEBUG_INFO("buff=%s\n",buff);
        if (!(opt=search_opt(buff,start))) continue;
//      DEBUG_INFO("p=%s\n",p);
        if (!str_2_opt(opt,p)) {
            fprintf(stderr,"invalid option value %s (%s:%d)\n",buff,file,n);
            continue;
        }
    }
    fclose(fp);

    return start;//返回指针地址
}

static int save_opts(opt_lq_type *opt_lq, const char *file, const char *mode, const char *comment)
{
    opt_lq_type *start = opt_lq;
    opt_lq_type *node = start;
    FILE *fp;
    char buff[2048];
    int i;
//  DEBUG_INFO("Save opt!comment is:%s\r\n", comment);

    if(start->opt.var == NULL) {
        DEBUG_ERROR("Sys opts is NULL");
        return -1;
    }

    if (!(fp = fopen(file, mode))) {
        DEBUG_ERROR("%s file open failed  or file opening", file);
        return -1;
    }
    if (comment) {
        fprintf(fp,"# %s\n\n", comment);
    }

    while (node) {
        opt_2_buf(&node->opt, buff);
        fprintf(fp, "%s\n", buff);
        node = node->next;
    }

    fclose(fp);
    sync();//确保及时写入磁盘
    return 0;
}
int save_opt(const char *file, opt_lq_type *opt_lq, const char *comment)
{
    return save_opts(opt_lq, file, "w", comment);//保存修改后的系统配置到文件
}

//下面为system配置文件操作函数
opt_info_type *opt_info;
opt_info_type opt_base;
opt_info_type *get_sysopts()
{
    if (opt_info)
    {
        return opt_info;
    }
    else
    {
        DEBUG_ERROR("opt_info is NULL\r\n");
        exit(1);
    }
}
opt_info_type *system_conf_init(const char *file)
{
    opt_lq_info_node = add_one_opts(&opt_lq_info_base,
    "sta-flag",                FORMAT_INIT,    (void *)&opt_base.sta.flag,                 "0:no;1:yes");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-led",                 FORMAT_INIT,    (void *)&opt_base.sta.led,                  "Consistent with led.");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-key",                 FORMAT_INIT,    (void *)&opt_base.sta.key,                  "Consistent with key.");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-audio",               FORMAT_INIT,    (void *)&opt_base.sta.audio,                "0:ok;1:overflow;0xff:i2c error;0xfe:i2s error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-upgrade",             FORMAT_INIT,    (void *)&opt_base.sta.upgrade,              "0:idle;1:data board;2:gnss board;3:4g;4:bluetooth");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-reg",                 FORMAT_INIT,    (void *)&opt_base.sta.reg,                  "0:valid;1:invalid");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-server",              FORMAT_INIT,    (void *)&opt_base.sta.server,               "0:ok;0xX1 back server error;0x1X cors error;0x11 all error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-login",               FORMAT_INIT,    (void *)&opt_base.sta.login,                "0:ok;1 no data or no reg;2:timeout");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-position",            FORMAT_INIT,    (void *)&opt_base.sta.position,             "0:ok;1:data error;2:gnss board error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-gnss_rover",          FORMAT_INIT,    (void *)&opt_base.sta.gnss_rover,           "0:ok;1 cors error;2:connect error;3:user error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-gnss_static",         FORMAT_INIT,    (void *)&opt_base.sta.gnss_static,          "0:ok;1 no data;2 no rom");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-detect",              FORMAT_INIT,    (void *)&opt_base.sta.detect,               "0:ok;1 electricity error;2:4g error;3:gnss board error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-warn",                FORMAT_INIT,    (void *)&opt_base.sta.warn,                 "0:ok;1 electric fence error;2:position error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-connect",             FORMAT_INIT,    (void *)&opt_base.sta.connect,              "0:ok;1 zhdcmd error");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sta-test",                FORMAT_INIT,    (void *)&opt_base.sta.test,                 "0:ok;1 get position data error");

    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-reset",               FORMAT_INIT,    (void *)&opt_base.sys.reset,                "0:normal;1:reset");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-version",             FORMAT_STRING,  (void *)opt_base.sys.version,               "1.0.0");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-conf_ver",            FORMAT_STRING,  (void *)opt_base.sys.conf_ver,              "1.0.0");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-dev_num",             FORMAT_STRING,  (void *)opt_base.sys.dev_num,               "13200000");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-dev_num_c",           FORMAT_INIT,    (void *)&opt_base.sys.dev_num_c,            "20");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-reg_num",             FORMAT_STRING,  (void *)opt_base.sys.reg_num,               "123456789012345678901234(24 ASCII)");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-reg_time",            FORMAT_STRING,  (void *)opt_base.sys.reg_time,              "2017.12.31");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-last_update_time",    FORMAT_STRING,  (void *)opt_base.sys.last_update_time,      "2018-7-13-10:29:13");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-reg_flag",            FORMAT_INIT,    (void *)&opt_base.sys.reg_flag,             "0:valid;1:invalid");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-product_name",        FORMAT_STRING,  (void *)opt_base.sys.product_name,          "qbox10-CMN");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-company_name",        FORMAT_STRING,  (void *)opt_base.sys.company_name,          "hi-target");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-product_type",        FORMAT_INIT,    (void *)&opt_base.sys.product_type,         "56:qbox8;66:qbox8-u;67:qbox10");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-product_type_sub",    FORMAT_INIT,    (void *)&opt_base.sys.product_type_sub,     "0:qbox10-CMN;1:qbox10-CMR");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-work_mode",           FORMAT_INIT,    (void *)&opt_base.sys.work_mode,            "0:base;1:rover;2:static;3:rover_static;4:single");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-gnssboard_version",   FORMAT_STRING,  (void *)opt_base.sys.gnssboard_version,     "OEM060604RN0000");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-gnssboard_type",      FORMAT_INIT,    (void *)&opt_base.sys.gnssboard_type,       "0:novtel;1:ublox");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-gnssboard_reset",     FORMAT_INIT,    (void *)&opt_base.sys.gnssboard_reset,      "0:normal;1:reset");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-ant_type",            FORMAT_INIT,    (void *)&opt_base.sys.ant_type,             "0:normal");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-netmodule_type",      FORMAT_INIT,    (void *)&opt_base.sys.netmodule_type,       "0:zte3630");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-bt_type",             FORMAT_INIT,    (void *)&opt_base.sys.bt_type,              "0:normal;1:rfcomm");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-bt_reset",            FORMAT_INIT,    (void *)&opt_base.sys.bt_reset,             "0:normal;1:reset");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-connect_type",        FORMAT_INIT,    (void *)&opt_base.sys.connect_type,         "0:bt;1:wifi");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-gga_record",          FORMAT_INIT,    (void *)&opt_base.sys.gga_record,           "0:no record;1:record");
    opt_lq_info_node = add_one_opts(opt_lq_info_node,
    "sys-volume",              FORMAT_INIT,    (void *)&opt_base.sys.volume,               "1-10/volume");

    myfree(opt_lq_info_node->prev->next);//最后一个分配的释放掉,一定要做这个释放操作

    load_opts(file, &opt_lq_info_base);//更新配置文件的数据到内存
    opt_info = &opt_base;
    return &opt_base;
}
int save_system_opt()
{
    int32_t ret = 0;
    pthread_mutex_lock(&opt_info->lock);
    ret = save_opts(&opt_lq_info_base, SYSTEM_CONFIG_DIR, "w", HEADER_COMMENT);//保存修改后的系统配置到文件
    pthread_mutex_unlock(&opt_info->lock);
    return ret;
}

void opts_write(uint32_t opt_type, void *val)
{
    pthread_mutex_lock(&opt_info->lock);
    switch (opt_type) {
        /*******下面时sta类型配置*********/
        case OPT_STA_FLAG:
            opt_info->sta.flag = *(uint32_t *)val;
            break;
        case OPT_STA_LED:
            opt_info->sta.led = *(uint32_t *)val;
            break;
        case OPT_STA_KEY:
            opt_info->sta.key = *(uint32_t *)val;
            break;
        case OPT_STA_AUDIO:
            opt_info->sta.audio = *(uint32_t *)val;
            break;
        case OPT_STA_UPGRADE:
            opt_info->sta.upgrade = *(uint32_t *)val;
            break;
        case OPT_STA_REG:
            opt_info->sta.reg = *(uint32_t *)val;
            break;
        case OPT_STA_SERVER:
            opt_info->sta.server = *(uint32_t *)val;
            break;
        case OPT_STA_LOGIN:
            opt_info->sta.login = *(uint32_t *)val;
            break;
        case OPT_STA_POSITION:
            opt_info->sta.position = *(uint32_t *)val;
            break;
        case OPT_STA_GNSS_ROVER:
            opt_info->sta.gnss_rover = *(uint32_t *)val;
            break;
        case OPT_STA_GNSS_STATIC:
            opt_info->sta.gnss_static = *(uint32_t *)val;
            break;
        case OPT_STA_DETECT:
            opt_info->sta.detect = *(uint32_t *)val;
            break;
        case OPT_STA_WARN:
            opt_info->sta.warn = *(uint32_t *)val;
            break;
        case OPT_STA_CONNECT:
            opt_info->sta.connect = *(uint32_t *)val;
            break;
        case OPT_STA_TEST:
            opt_info->sta.test = *(uint32_t *)val;
            break;
        /*******上面时sta类型配置*********/
        /*******下面时sys类型配置*********/
        case OPT_SYS_RESET:
            opt_info->sys.reset = *(uint32_t *)val;
            break;
        case OPT_SYS_VERSION:
            memcpy(opt_info->sys.version, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_CONF_VER:
            memcpy(opt_info->sys.conf_ver, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_DEV_NUM:
            memcpy(opt_info->sys.dev_num, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_REG_NUM:
            memcpy(opt_info->sys.reg_num, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_REG_TIME:
            memcpy(opt_info->sys.reg_time, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_REG_FLAG:
            opt_info->sys.reg_flag = *(uint32_t *)val;
            break;
        case OPT_SYS_DEV_TYPE:
            opt_info->sys.dev_type = *(uint32_t *)val;
            break;
        case OPT_SYS_PRODUCT_NAME:
            memcpy(opt_info->sys.product_name, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_COMPANY_NAME:
            memcpy(opt_info->sys.company_name, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_PRODUCT_TYPE:
            opt_info->sys.product_type = *(uint32_t *)val;
            break;
        case OPT_SYS_PRODUCT_TYPE_SUB:
            opt_info->sys.product_type_sub = *(uint32_t *)val;
            break;
        case OPT_SYS_WORK_MODE:
            opt_info->sys.work_mode = *(uint32_t *)val;
            break;
        case OPT_SYS_GNSSBOARD_VERSION:
            memcpy(opt_info->sys.gnssboard_version, (int8_t *)val, strlen((int8_t *)val));
            break;
        case OPT_SYS_GNSSBOARD_TYPE:
            opt_info->sys.gnssboard_type = *(uint32_t *)val;
            break;
        case OPT_SYS_GNSSBOARD_RESET:
            opt_info->sys.gnssboard_reset = *(uint32_t *)val;
            break;
        case OPT_SYS_ANT_TYPE:
            opt_info->sys.ant_type = *(uint32_t *)val;
            break;
        case OPT_SYS_NETMODULE_TYPE:
            opt_info->sys.netmodule_type = *(uint32_t *)val;
            break;
        case OPT_SYS_BT_TYPE:
            opt_info->sys.bt_type = *(uint32_t *)val;
            break;
        case OPT_SYS_BT_RESET:
            opt_info->sys.bt_reset = *(uint32_t *)val;
            break;
        case OPT_SYS_CONNECT_TYPE:
            opt_info->sys.connect_type = *(uint32_t *)val;
            break;
    }
    pthread_mutex_unlock(&opt_info->lock);
}

