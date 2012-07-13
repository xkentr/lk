/*
 * Copyright (c) 2012 Travis Geiselbrecht
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include <debug.h>
#include <err.h>
#include <sys/types.h>
#include <kernel/thread.h>
#include <platform.h>
#include <platform/timer.h>
#include <stm32f10x_rcc.h>
#include <stm32f10x_tim.h>
#include <misc.h>
#include <arch/arm/cm3.h>

#define LOCAL_TRACE 0

#define TIME_BASE_COUNT 0xffff
#define TICK_RATE 1000000

static volatile uint64_t ticks = 0;

static platform_timer_callback cb;
static void *cb_args;

/* use systick as the kernel tick */
void _systick(void)
{
	inc_critical_section();

	bool resched = false;
	ticks += 10;
	if (cb) {
		if (cb(cb_args, ticks) == INT_RESCHEDULE)
			resched = true;
	}

	if (resched) {
		// have the cortex-m3 queue a preemption
		cm3_trigger_preempt();
	}

	dec_critical_section();
}

status_t platform_set_periodic_timer(platform_timer_callback callback, void *arg, time_t interval)
{
	LTRACEF("callback %p, arg %p, interval %d\n", callback, arg, interval);

	cb = callback;
	cb_args = arg;

    RCC_ClocksTypeDef clocks;
    RCC_GetClocksFreq(&clocks);

	cm3_systick_set_periodic(clocks.SYSCLK_Frequency, interval);

	return NO_ERROR;
}

static void stm32_tim_irq(uint num)
{
	TRACEF("tim irq %d\n", num);
	PANIC_UNIMPLEMENTED;
}

void stm32_tim3_irq(void)
{
	stm32_tim_irq(3);
}

void stm32_tim4_irq(void)
{
	stm32_tim_irq(4);
}

void stm32_tim5_irq(void)
{
	stm32_tim_irq(5);
}

void stm32_tim6_irq(void)
{
	stm32_tim_irq(6);
}

void stm32_tim7_irq(void)
{
	stm32_tim_irq(7);
}

void stm32_tim2_irq(void)
{
	stm32_tim_irq(2);
}

time_t current_time(void)
{
	return ticks;
}

bigtime_t current_time_hires(void)
{
	uint64_t tusec;
	uint32_t count1, count2;
	uint32_t reload = SysTick->LOAD  & SysTick_LOAD_RELOAD_Msk;

	// The tick count can roll over while we read the counter,
	// so try to prevent that.
	do {
		count1 = (volatile uint32_t)SysTick->VAL;
		enter_critical_section();
		count2 = (volatile uint32_t)SysTick->VAL;
		tusec = (volatile uint64_t)ticks;
		exit_critical_section();
	} while (count2 > count1);

	tusec = tusec * 1000;

	RCC_ClocksTypeDef clocks;
	RCC_GetClocksFreq(&clocks);
	uint32_t clk_mhz = clocks.SYSCLK_Frequency / 1000000;
	count1 = reload - count1;
	count1 /= clk_mhz;

	return tusec + count1;
}

void stm32_timer_early_init(void)
{
}

void stm32_timer_init(void)
{
}
