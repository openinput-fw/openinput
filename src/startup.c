#include <stm32f10x.h>

extern void _estack(); // Not really a function, just to be compatible with array later

extern uint32_t _svect; // ISR Vectors
extern uint32_t _evect;

extern uint32_t _stext; // Main program
extern uint32_t _etext;

extern uint32_t _siiram0; // RAM code source
extern uint32_t _siram0; // RAM code destination
extern uint32_t _eiram0;

extern uint32_t _sidata; // Data source
extern uint32_t _sdata; // Data destination
extern uint32_t _edata;

extern uint32_t _sbss; // BSS destination
extern uint32_t _ebss;

extern uint32_t _end;


void _default_isr()
{
    while(1);
}

void __attribute__ ((weak)) __libc_init_array()
{

}

extern int init();
extern int main();

#define DEFAULT_ISR "_default_isr"

void _reset_isr()
{
    uint32_t *src, *dst;

    src = &_sidata;
    dst = &_sdata;

    while (dst < &_edata) // Copy data
        *(dst++) = *(src++);

    src = 0;
    dst = &_sbss;

    while (dst < &_ebss) // Zero BSS
        *(dst++) = 0;

    __libc_init_array();

    SCB->VTOR = (uint32_t)&_svect; // ISR Vectors offset
    SCB->AIRCR = 0x05FA0000 | (4 << 8); // Interrupt priority - 3 bits Group, 1 bit Sub-group
    SCB->SHCSR = SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk; // Enable separate fault handlers

    init();
    main();

    __disable_irq();
    while(1);
}

void _nmi_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _hardfault_isr()                     __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _memmanage_isr()                     __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _busfault_isr()                      __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usagefault_isr()                    __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _svc_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _debugmon_isr()                      __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _pendsv_isr()                        __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _systick_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _wwdg_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _pvd_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tamper_stamp_isr()                  __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _rtc_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _flash_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _rcc_isr()                           __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti0_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti1_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti2_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti3_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti4_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel1_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel2_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel3_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel4_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel5_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel6_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma1_channel7_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _adc1_2_isr()                        __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usb_hp_can_tx_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usb_lp_can_rx0_isr()                __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _can_rx1_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _can_sce_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti9_5_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim1_brk_isr()                      __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim1_up_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim1_trg_com_isr()                  __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim1_cc_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim2_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim3_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim4_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _i2c1_ev_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _i2c1_er_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _i2c2_ev_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _i2c2_er_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _spi1_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _spi2_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usart1_isr()                        __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usart2_isr()                        __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usart3_isr()                        __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _exti15_10_isr()                     __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _rtc_alarm_isr()                     __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _usb_wakeup_isr()                    __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim8_brk_isr()                      __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim8_up_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim8_trg_com_isr()                  __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim8_cc_isr()                       __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _adc3_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _fsmc_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _sdio_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim5_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _spi3_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _uart4_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _uart5_isr()                         __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim6_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _tim7_isr()                          __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma2_channel1_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma2_channel2_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma2_channel3_isr()                 __attribute__ ((weak,  alias (DEFAULT_ISR)));
void _dma2_channel4_5_isr()               __attribute__ ((weak,  alias (DEFAULT_ISR)));

__attribute__ ((section(".isr_vector"))) void (* const g_pfnVectors[])() = {
    _estack,
    _reset_isr,
    _nmi_isr,
    _hardfault_isr,
    _memmanage_isr,
    _busfault_isr,
    _usagefault_isr,
    0,
    0,
    0,
    0,
    _svc_isr,
    _debugmon_isr,
    0,
    _pendsv_isr,
    _systick_isr,
    _wwdg_isr,
    _pvd_isr,
    _tamper_stamp_isr,
    _rtc_isr,
    _flash_isr,
    _rcc_isr,
    _exti0_isr,
    _exti1_isr,
    _exti2_isr,
    _exti3_isr,
    _exti4_isr,
    _dma1_channel1_isr,
    _dma1_channel2_isr,
    _dma1_channel3_isr,
    _dma1_channel4_isr,
    _dma1_channel5_isr,
    _dma1_channel6_isr,
    _dma1_channel7_isr,
    _adc1_2_isr,
    _usb_hp_can_tx_isr,
    _usb_lp_can_rx0_isr,
    _can_rx1_isr,
    _can_sce_isr,
    _exti9_5_isr,
    _tim1_brk_isr,
    _tim1_up_isr,
    _tim1_trg_com_isr,
    _tim1_cc_isr,
    _tim2_isr,
    _tim3_isr,
    _tim4_isr,
    _i2c1_ev_isr,
    _i2c1_er_isr,
    _i2c2_ev_isr,
    _i2c2_er_isr,
    _spi1_isr,
    _spi2_isr,
    _usart1_isr,
    _usart2_isr,
    _usart3_isr,
    _exti15_10_isr,
    _rtc_alarm_isr,
    _usb_wakeup_isr,
    _tim8_brk_isr,
    _tim8_up_isr,
    _tim8_trg_com_isr,
    _tim8_cc_isr,
    _adc3_isr,
    _fsmc_isr,
    _sdio_isr,
    _tim5_isr,
    _spi3_isr,
    _uart4_isr,
    _uart5_isr,
    _tim6_isr,
    _tim7_isr,
    _dma2_channel1_isr,
    _dma2_channel2_isr,
    _dma2_channel3_isr,
    _dma2_channel4_5_isr,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0
};
