#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include "itsmsg_codec.h"

extern void ex_itsmsg_encode();
extern void ex_itsmsg_decode();

#ifdef __THREADX__
#include <tx_api.h>
/* thread */
#define TX_THREAD_PRIORITY 60
static TX_THREAD tx_thread;
static uint8_t tx_thread_stack[0x3000];
#define ITSMSG_TEST_POOL_BUF_SZ 1024 * 1024 * 4
static uint8_t pool_buf[ITSMSG_TEST_POOL_BUF_SZ];
#endif

void itsmsg_example(unsigned long input)
{
    /* Not using input */
    (void)input;
    int ret;
    ITSMsgConfig cfg;

#ifdef __THREADX__
    cfg.mem_pool_buf = pool_buf;
    cfg.mem_pool_sz = ITSMSG_TEST_POOL_BUF_SZ;
#endif
    ret = itsmsg_init(&cfg);
    if (ret != 0) {
        return;
    }

    ex_itsmsg_decode();
    ex_itsmsg_encode();

    return;
}

#ifdef __CRATON__
void craton_user_init(void)
#else
int main(int argc, char **argv)
#endif
{
    (void)argc;
    (void)argv;
#ifdef __THREADX__
    /* ThreadX return value */
    ULONG trv = TX_SUCCESS;

    /* Create TX thread */
    trv = tx_thread_create(&tx_thread, "ex_encode",
                           itsmsg_example, 0,
                           tx_thread_stack,
                           sizeof(tx_thread_stack),
                           TX_THREAD_PRIORITY,
                           TX_THREAD_PRIORITY,
                           TX_NO_TIME_SLICE, TX_AUTO_START);
    assert(trv == TX_SUCCESS);

    return;
#else
    itsmsg_example(0);
    return 0;
#endif
}
