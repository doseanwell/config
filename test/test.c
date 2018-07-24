/**************************************************************
* 文件名:
*   main.c
*
*   描述:
*
*
* 版本：V0.0.1：由sean创建
***************************************************************/
#include "option.h"

#define MAIN_CONFIG_DIR "./main.conf"
typedef struct {
    uint8_t gpstest_flag;//测试总标志
    uint8_t gga_flag;//gga测试标志
    uint8_t workmode_flag;//gga测试标志
    uint8_t nettest_flag;//gga测试标志
} main_opt_type;

main_opt_type main_opt;
opt_lq_type opt_lq_main_base, *opt_lq_main_node;
static void main_opt_init()
{
    opt_lq_main_node = add_one_opts(&opt_lq_main_base,
                                    "main-gpstest",         FORMAT_INIT, (void *)&main_opt.gpstest_flag,    "0:close;1:open");
    opt_lq_main_node = add_one_opts(opt_lq_main_node,
                                    "main-gga",             FORMAT_INIT, (void *)&main_opt.gga_flag,        "0:close;1:open");
    opt_lq_main_node = add_one_opts(opt_lq_main_node,
                                    "main-workmode",        FORMAT_INIT, (void *)&main_opt.workmode_flag,   "0:base;1:rover;2:static;3:rover_static;4:single;5:null");
    opt_lq_main_node = add_one_opts(opt_lq_main_node,
                                    "main-nettest",         FORMAT_INIT, (void *)&main_opt.nettest_flag,    "0:close;1:open");
    myfree(opt_lq_main_node->prev->next);//最后一个分配的释放掉,一定要做这个释放操作

    load_opts(MAIN_CONFIG_DIR, &opt_lq_main_base);//更新配置文件的数据到内存

    printf("main-gpstest is : %d\n", main_opt.gpstest_flag);
    printf("main-gga is : %d\n", main_opt.gga_flag);
    printf("main-workmode is : %d\n", main_opt.workmode_flag);
    printf("main-nettest is : %d\n", main_opt.nettest_flag);

    save_opt(MAIN_CONFIG_DIR, &opt_lq_main_base, "Qbox10 main config");
}

/*******************************************************************************
 * 名称: main
 * 功能: 进程主函数
 * 形参: argc：参数个数
 *      argv：参数数据指向数组的指针
 * 返回: 无
 * 说明: 无
 ******************************************************************************/
int main(int argc, char *argv[])
{
    main_opt_init();
    while(1) {
        usleep(1000000);
    }
}

