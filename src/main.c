#include <msp430.h>
#include <stdlib.h>
#include "os_applAPI.h"

static void blink_task(void)
{
	task_open();
	for(;;) {
		P4OUT ^= BIT6;
		task_wait(10);
	}
	task_close();
}

void system_init(void)
{
	WDTCTL = WDTPW | WDTHOLD;

	P1DIR = BIT0;
	P1OUT = BIT0;
	P4DIR = BIT6;
	P4OUT = 0;

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
	task_create(blink_task, 1, NULL, 0, 0);
	os_start();

	P4OUT |= BIT6;
	return 0;
}

__attribute__ ((interrupt(TIMER0_A0_VECTOR))) void TIMER0_A0_ISR(void)
{
	os_tick();
	P1OUT ^= BIT0;
	__low_power_mode_off_on_exit();
	__eint();
}
