MEMORY
{
    rom  (rx)     : ORIGIN = 0x08002000, LENGTH = 0x06000
    ram  (rw)     : ORIGIN = 0x20000000, LENGTH = 0x02800
}

INCLUDE stm32f1-common.ld
