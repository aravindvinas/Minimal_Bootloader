#include "main.h"

int 
main(void)
{
    jump_user();
}

static void jump_user(void)
{
    uint32_t user_base = *(volatile uint32_t *) (USER_BASE);
    uint32_t user_msp = user_base;
    void (*user_reset) (void) = (void *) (user_base + 4);
    __set_MSP(user_msp);
    user_reset();
}
