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
int *delect_one_opts(const char *file)
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
        printf("The %s file exist\n", file);
        if (!(fp = fopen(file,"r"))) {
                printf("%s is none\n",file);
                return -1;
        }
        if (fgets(buff,sizeof(buff),fp)) {//做个保护,说明有数据，直接返回
            fclose(fp);
            return 0;
        }
    }

    if (!(fp = fopen(file,"w+"))) {
        printf("%s is none\n",file);
        return -1;
    }

    printf("%s creat success!\n",file);

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
        printf("%s is error\n",file);
        return NULL;
    }
    node = start;

    if (!(fp=fopen(file,"r"))) {
        printf("%s is none\n",file);
        return NULL;
    }

    while (fgets(buff,sizeof(buff),fp)) {
//      printf("buff=%s\n",buff);
        n++;
        chop(buff);

        if (buff[0]=='\0') continue;

        if (!(p=strstr(buff,"="))) {
            fprintf(stderr,"invalid option %s (%s:%d)\n",buff,file,n);
            continue;
        }
        *p++='\0';
        chop(buff);
//      printf("buff=%s\n",buff);
        if (!(opt=search_opt(buff,start))) continue;
//      printf("p=%s\n",p);
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
//  printf("Save opt!comment is:%s\r\n", comment);

    if(start->opt.var == NULL) {
        printf("Sys opts is NULL");
        return -1;
    }

    if (!(fp = fopen(file, mode))) {
        printf("%s file open failed  or file opening", file);
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
