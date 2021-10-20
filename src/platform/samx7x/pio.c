/*
 * SPDX-License-Identifier: MIT
 * SPDX-FileCopyrightText: 2021 Rafael Silva <perigoso@riseup.net>
 */

#include <sam.h>

#include "platform/samx7x/pio.h"
#include "platform/samx7x/pmc.h"
#include "util/data.h"

/*
 * resolves to the base address of the PIO peripheral
 * 0x00000200 is the size/offset of each PIO peripheral in memory
 */
#define _PIO(port) ((Pio *) ((u32) PIOA + (u32) (port * 0x00000200UL)))

/*
 * Helps set the peripheral mux (its split in 2 regs)
 * port -> PIO peripheral number (not address!)
 * pin -> pin number
 * sel -> selection
 */
#define _PIO_PERI_MUX_SELECT(port, pin, sel)                                                                \
	_PIO(port)->PIO_ABCDSR[0] = (_PIO(port)->PIO_ABCDSR[0] & ~BIT(pin)) | (!!((sel) &BIT(0)) << (pin)); \
	_PIO(port)->PIO_ABCDSR[1] = (_PIO(port)->PIO_ABCDSR[1] & ~BIT(pin)) | (!!((sel) &BIT(1)) << (pin));

void pio_init()
{
/**
	 * PORT A
	 */
#if defined(PIOA)
	pmc_peripheral_clock_gate(PIOA_CLOCK_ID, 1); // Enable peripheral clock

	/** OUTPUT **/
	/* Multi-drive (open-drain) enable (ER) or disable (DR) */
	PIOA->PIO_MDDR = 0xFFFFFFFF;
	/* Output set enable (ER) or disable (DR) */
	PIOA->PIO_OWDR = 0xFFFFFFFF;
	/* Output enable (ER) or disable (DR) */
	PIOA->PIO_ODR = 0xFFFFFFFF;
	/* High drive enable */
	PIOA->PIO_DRIVER = 0;

	/** INPUT **/
	/* Input filter enable (ER) or disable (DR) */
	PIOA->PIO_IFDR = 0xFFFFFFFF;
	/* Slow filter enable (ER) or disable (DR) */
	PIOA->PIO_IFSCDR = 0xFFFFFFFF;

	/** PULL **/
	/* Pull-up enable (ER) or disable (DR) */
	PIOA->PIO_PUDR = 0xFFFFFFFF;
	/* Pull-down enable (ER) or disable (DR) */
	PIOA->PIO_PPDER = 0xFFFFFFFF;

	/** PERIPHERALS **/
	/* GPIO (ER) or Peripheral control (DR) */
	PIOA->PIO_PER = 0xFFFFFFFF;

	/** INTERRUPTS **/
	/* Interrupt enable (ER) / disable (DR) */
	PIOA->PIO_IDR = 0xFFFFFFFF;
	/* Advanced interrupt enable (ER) or disable (DR) */
	PIOA->PIO_AIMDR = 0xFFFFFFFF;

#endif

	/**
	 * PORT B
	 */
#if defined(PIOB)
	pmc_peripheral_clock_gate(PIOB_CLOCK_ID, 1); // Enable peripheral clock

	/** OUTPUT **/
	/* Multi-drive (open-drain) enable (ER) or disable (DR) */
	PIOB->PIO_MDDR = 0xFFFFFFFF;
	/* Output set enable (ER) or disable (DR) */
	PIOB->PIO_OWDR = 0xFFFFFFFF;
	/* Output enable (ER) or disable (DR) */
	PIOB->PIO_ODR = 0xFFFFFFFF;
	/* High drive enable */
	PIOB->PIO_DRIVER = 0;

	/** INPUT **/
	/* Input filter enable (ER) or disable (DR) */
	PIOB->PIO_IFDR = 0xFFFFFFFF;
	/* Slow filter enable (ER) or disable (DR) */
	PIOB->PIO_IFSCDR = 0xFFFFFFFF;

	/** PULL **/
	/* Pull-up enable (ER) or disable (DR) */
	PIOB->PIO_PUDR = 0xFFFFFFFF;
	/* Pull-down enable (ER) or disable (DR) */
	PIOB->PIO_PPDER = 0xFFFFFFFF;

	/** PERIPHERALS **/
	/* GPIO (ER) or Peripheral control (DR) */
	PIOB->PIO_PER = 0xFFFFFFFF;

	/** INTERRUPTS **/
	/* Interrupt enable (ER) / disable (DR) */
	PIOB->PIO_IDR = 0xFFFFFFFF;
	/* Advanced interrupt enable (ER) or disable (DR) */
	PIOB->PIO_AIMDR = 0xFFFFFFFF;

#endif

	/**
	 * PORT C
	 */
#if defined(PIOC)
	pmc_peripheral_clock_gate(PIOC_CLOCK_ID, 1); // Enable peripheral clock

	/** OUTPUT **/
	/* Multi-drive (open-drain) enable (ER) or disable (DR) */
	PIOC->PIO_MDDR = 0xFFFFFFFF;
	/* Output set enable (ER) or disable (DR) */
	PIOC->PIO_OWDR = 0xFFFFFFFF;
	/* Output enable (ER) or disable (DR) */
	PIOC->PIO_ODR = 0xFFFFFFFF;
	/* High drive enable */
	PIOC->PIO_DRIVER = 0;

	/** INPUT **/
	/* Input filter enable (ER) or disable (DR) */
	PIOC->PIO_IFDR = 0xFFFFFFFF;
	/* Slow filter enable (ER) or disable (DR) */
	PIOC->PIO_IFSCDR = 0xFFFFFFFF;

	/** PULL **/
	/* Pull-up enable (ER) or disable (DR) */
	PIOC->PIO_PUDR = 0xFFFFFFFF;
	/* Pull-down enable (ER) or disable (DR) */
	PIOC->PIO_PPDER = 0xFFFFFFFF;

	/** PERIPHERALS **/
	/* GPIO (ER) or Peripheral control (DR) */
	PIOC->PIO_PER = 0xFFFFFFFF;

	/** INTERRUPTS **/
	/* Interrupt enable (ER) / disable (DR) */
	PIOC->PIO_IDR = 0xFFFFFFFF;
	/* Advanced interrupt enable (ER) or disable (DR) */
	PIOC->PIO_AIMDR = 0xFFFFFFFF;

#endif

	/**
	 * PORT D
	 */
#if defined(PIOD)
	pmc_peripheral_clock_gate(PIOD_CLOCK_ID, 1); // Enable peripheral clock

	/** OUTPUT **/
	/* Multi-drive (open-drain) enable (ER) or disable (DR) */
	PIOD->PIO_MDDR = 0xFFFFFFFF;
	/* Output set enable (ER) or disable (DR) */
	PIOD->PIO_OWDR = 0xFFFFFFFF;
	/* Output enable (ER) or disable (DR) */
	PIOD->PIO_ODR = 0xFFFFFFFF;
	/* High drive enable */
	PIOD->PIO_DRIVER = 0;

	/** INPUT **/
	/* Input filter enable (ER) or disable (DR) */
	PIOD->PIO_IFDR = 0xFFFFFFFF;
	/* Slow filter enable (ER) or disable (DR) */
	PIOD->PIO_IFSCDR = 0xFFFFFFFF;

	/** PULL **/
	/* Pull-up enable (ER) or disable (DR) */
	PIOD->PIO_PUDR = 0xFFFFFFFF;
	/* Pull-down enable (ER) or disable (DR) */
	PIOD->PIO_PPDER = 0xFFFFFFFF;

	/** PERIPHERALS **/
	/* GPIO (ER) or Peripheral control (DR) */
	PIOD->PIO_PER = 0xFFFFFFFF;

	/** INTERRUPTS **/
	/* Interrupt enable (ER) / disable (DR) */
	PIOD->PIO_IDR = 0xFFFFFFFF;
	/* Advanced interrupt enable (ER) or disable (DR) */
	PIOD->PIO_AIMDR = 0xFFFFFFFF;

#endif

	/**
	 * PORT E
	 */
#if defined(PIOE)
	pmc_peripheral_clock_gate(PIOE_CLOCK_ID, 1); // Enable peripheral clock

	/** OUTPUT **/
	/* Multi-drive (open-drain) enable (ER) or disable (DR) */
	PIOE->PIO_MDDR = 0xFFFFFFFF;
	/* Output set enable (ER) or disable (DR) */
	PIOE->PIO_OWDR = 0xFFFFFFFF;
	/* Output enable (ER) or disable (DR) */
	PIOE->PIO_ODR = 0xFFFFFFFF;
	/* High drive enable */
	PIOE->PIO_DRIVER = 0;

	/** INPUT **/
	/* Input filter enable (ER) or disable (DR) */
	PIOE->PIO_IFDR = 0xFFFFFFFF;
	/* Slow filter enable (ER) or disable (DR) */
	PIOE->PIO_IFSCDR = 0xFFFFFFFF;

	/** PULL **/
	/* Pull-up enable (ER) or disable (DR) */
	PIOE->PIO_PUDR = 0xFFFFFFFF;
	/* Pull-down enable (ER) or disable (DR) */
	PIOE->PIO_PPDER = 0xFFFFFFFF;

	/** PERIPHERALS **/
	/* GPIO (ER) or Peripheral control (DR) */
	PIOE->PIO_PER = 0xFFFFFFFF;

	/** INTERRUPTS **/
	/* Interrupt enable (ER) / disable (DR) */
	PIOE->PIO_IDR = 0xFFFFFFFF;
	/* Advanced interrupt enable (ER) or disable (DR) */
	PIOE->PIO_AIMDR = 0xFFFFFFFF;

#endif
}

void pio_config(
	struct pio_pin_t pin,
	enum pio_direction_t direction,
	u8 out_data,
	enum pio_pull_t pull,
	enum pio_mux_t mux,
	enum pio_config_flags_t config)
{
	pio_direction(pin, direction);
	pio_pull(pin, pull);

	if (config & PIO_OPEN_DRAIN) {
		_PIO(pin.port)->PIO_MDER = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_MDDR = BIT(pin.pin);
	}

	if (config & PIO_HIGH_DRIVE) {
		_PIO(pin.port)->PIO_DRIVER = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_DRIVER &= ~BIT(pin.pin);
	}

	if (config & PIO_FILTER) {
		_PIO(pin.port)->PIO_IFER = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_IFDR = BIT(pin.pin);
	}

	if (config & PIO_SLOW_FILTER) {
		_PIO(pin.port)->PIO_IFSCER = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_IFSCDR = BIT(pin.pin);
	}

	pio_set(pin, out_data);
	pio_peripheral_mux(pin, mux);
	pio_peripheral_control(pin, config & PIO_PERIPHERAL_CTRL);
}

void pio_peripheral_mux(struct pio_pin_t pin, enum pio_mux_t mux)
{
	_PIO_PERI_MUX_SELECT(pin.port, pin.pin, mux);
}

void pio_peripheral_control(struct pio_pin_t pin, u8 enable)
{
	if (enable) {
		_PIO(pin.port)->PIO_PDR = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_PER = BIT(pin.pin);
	}
}

void pio_direction(struct pio_pin_t pin, enum pio_direction_t direction)
{
	if (direction) // 1 = input
	{
		_PIO(pin.port)->PIO_ODR = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_OER = BIT(pin.pin);
	}
}

void pio_pull(struct pio_pin_t pin, enum pio_pull_t pull)
{
	if (pull & PIO_PULL_UP)
		_PIO(pin.port)->PIO_PUER = BIT(pin.pin);
	else
		_PIO(pin.port)->PIO_PUDR = BIT(pin.pin);

	if (pull & PIO_PULL_DOWN)
		_PIO(pin.port)->PIO_PPDER = BIT(pin.pin);
	else
		_PIO(pin.port)->PIO_PPDDR = BIT(pin.pin);
}

void pio_set(struct pio_pin_t pin, u8 state)
{
	if (state) {
		_PIO(pin.port)->PIO_SODR = BIT(pin.pin);
	} else {
		_PIO(pin.port)->PIO_CODR = BIT(pin.pin);
	}
}

u8 pio_get(struct pio_pin_t pin)
{
	return !!(_PIO(pin.port)->PIO_PDSR & BIT(pin.pin));
}
