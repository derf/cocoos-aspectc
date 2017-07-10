#include <msp430.h>
#include <stdlib.h>
#include "os_applAPI.h"

Evt_t leftButtonEvent;
Evt_t rightButtonEvent;
Sem_t ledSem;

[[OS::task()]]
static void blink_task(void)
{
	task_open();
	for (;;) {
		P4OUT ^= BIT6;
		task_wait(10);
	}
	task_close();
}

[[OS::task()]]
static void leftbutton_task(void)
{
	task_open();
	for (;;) {
		if (!(P4IN & BIT5)) {
			event_signal(leftButtonEvent);
		}
		task_wait(20);
	}
	task_close();
}

[[OS::task()]]
static void rightbutton_task(void)
{
	task_open();
	for (;;) {
		if (!(P1IN & BIT1)) {
			event_signal(rightButtonEvent);
		}
		task_wait(20);
	}
	task_close();
}

[[OS::task()]]
static void led_task(void)
{
	task_open();
	for (;;) {
		event_wait(rightButtonEvent);
		P1OUT ^= BIT0;
	}
	task_close();
}

void system_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	P1DIR = BIT0;
	P1REN = BIT1;
	P1OUT = BIT1;
	P4DIR = BIT6;
	P4REN = BIT5;
	P4OUT = BIT5;

	P4IFG = 0;
	P4IE = BIT5;

	PJSEL0 = BIT4 | BIT5;

	PM5CTL0 &= ~LOCKLPM5;

	FRCTL0 = FWPW;
	FRCTL0_L = 0x10;
	FRCTL0_H = 0xff;

	CSCTL0_H = CSKEY >> 8;
	CSCTL1 = DCORSEL | DCOFSEL_4;
	CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
	CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
	CSCTL0_H = 0;

	TA0CTL = TASSEL__ACLK | ID_3 | MC__UP;
	TA0CTL |= TACLR;
	TA0CCR0 = 50;
	TA0CCTL0 |= CCIE;
}

int main(void)
{
	system_init();
	os_init();
	leftButtonEvent = event_create();
	rightButtonEvent = event_create();
	ledSem = sem_bin_create(0);
	task_create(blink_task, 1, NULL, 0, 0);
	task_create(leftbutton_task, 2, NULL, 0, 0);
	task_create(rightbutton_task, 3, NULL, 0, 0);
	task_create(led_task, 4, NULL, 0, 0);
	os_start();

	P4OUT |= BIT6;
	return 0;
}

#ifndef __acweaving
__attribute__((interrupt(TIMER0_A0_VECTOR))) __attribute((wakeup)) void TIMER0_A0_ISR(void)
{
	os_tick();
	__eint();
}
__attribute__((interrupt(PORT4_VECTOR))) void PORT4_ISR(void)
{
	P4IFG = 0;
	P1OUT ^= BIT0;
}
#endif
