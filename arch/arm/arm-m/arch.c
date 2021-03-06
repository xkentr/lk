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
#include <arch.h>
#include <arch/ops.h>
#include <arch/arm.h>
#include <platform.h>
#include <arch/arm/cm3.h>

#define ENABLE_CYCLE_COUNTER 1

extern void *vectab;

extern int _end_of_ram;
void *_heap_end = &_end_of_ram;

unsigned int cm3_num_irq_pri_bits;
unsigned int cm3_irq_pri_mask;

void arch_early_init(void)
{
	arch_disable_ints();

	/* set the vector table base */
	SCB->VTOR = (uint32_t)&vectab;

	/* clear any pending interrupts and set all the vectors to medium priority */
	int i;
	int groups = InterruptType->ICTR & 0xf;
	for (i = 0; i < groups; i++) {
		NVIC->ICER[i] = 0xffffffff;
		NVIC->ICPR[i] = 0xffffffff;
		NVIC->IP[i*4] = 128; /* medium priority */
		NVIC->IP[i*4+1] = 128; /* medium priority */
		NVIC->IP[i*4+2] = 128; /* medium priority */
		NVIC->IP[i*4+3] = 128; /* medium priority */
	}

	/* number of priorities */
	for (i=0; i < 7; i++) {
		__set_BASEPRI(1 << i);
		if (__get_BASEPRI() != 0)
			break;
	}
	cm3_num_irq_pri_bits = 8 - i;
	cm3_irq_pri_mask = ~((1 << i) - 1) & 0xff;

	/* leave BASEPRI at 0 */
	__set_BASEPRI(0);

	/* set priority grouping to 0 */
	NVIC_SetPriorityGrouping(0);

	/* enable certain faults */
	SCB->SHCSR |= (SCB_SHCSR_USGFAULTENA_Msk | SCB_SHCSR_BUSFAULTENA_Msk | SCB_SHCSR_MEMFAULTENA_Msk);

	/* set the svc and pendsv priority level to pretty low */
	SCB->SHP[11-4] = cm3_lowest_priority();
	SCB->SHP[14-4] = cm3_lowest_priority();

	/* initialize the systick mechanism */
	cm3_systick_init();
}

void arch_init(void)
{
#if ENABLE_CYCLE_COUNTER
	*REG32(SCB_DEMCR) |= 0x01000000; // global trace enable
	*REG32(DWT_CYCCNT) = 0;
	*REG32(DWT_CTRL) |= 1; // enable cycle counter
#endif
}

void arch_quiesce(void)
{
}

void arch_idle(void)
{
}

void cm3_set_irqpri(uint32_t pri)
{
	if (pri == 0) {
		__disable_irq(); // cpsid i
		__set_BASEPRI(0);
	} else if (pri >= 256) {
		__set_BASEPRI(0);
		__enable_irq();
	} else {
		uint32_t _pri = pri & cm3_irq_pri_mask;

		if (_pri == 0)
			__set_BASEPRI(1 << (8 - cm3_num_irq_pri_bits));
		else
			__set_BASEPRI(_pri);
		__enable_irq(); // cpsid i
	}
}

