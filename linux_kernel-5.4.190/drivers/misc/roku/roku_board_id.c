#include <linux/kernel.h>

// One-time setup (command line param) for global TV hardware; used in multiple places
static bool is_global_tv_hw = false;
static int __init rokutv_global_board_init(char *str)
{
    is_global_tv_hw = true;
    return 1;
}
__setup("rokutv_global_board", rokutv_global_board_init);

bool roku_is_global_hw(void)
{
    return is_global_tv_hw;
}
