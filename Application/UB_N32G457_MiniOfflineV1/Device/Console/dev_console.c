#include <dev_console.h>

////////////////////////////////////////////////////////////////////////////////
////
void* USART_Handle;

#define DEV_CONSOLE_RX_THREAD_STACK_SIZE    1024
#define DEV_CONSOLE_RX_THREAD_PRIORITY      20
#define DEV_CONSOLE_RX_THREAD_TIMESLICE     10

__ALIGNED(OS_ALIGN_SIZE)
static uint8_t rx_thread_stack[DEV_CONSOLE_RX_THREAD_STACK_SIZE];
static os_sem_t rx_sem;
static os_thread_t rx_thread;

static void dev_console_rx_thread_entry(void* p){
    while(1){
        os_sem_take(&rx_sem, OS_WAIT_INFINITY);

    }
}

////////////////////////////////////////////////////////////////////////////////
////


void dev_console_init(void* USARTx)
{
    USART_Handle = USARTx;
}

void dev_console_send(uint8_t * data, os_size_t data_size);

os_err_t dev_console_recv(dev_console_rx_handler rx_handler, void* userdata);