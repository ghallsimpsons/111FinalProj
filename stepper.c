/* Grantland Hall 2014
 * GrantlandHall@berkeley.edu
 */
#include <msp430.h>
#include <stdlib.h>

int ctr; // Timer counter
int step; // how long between steps
int steps[] = {BIT0+BIT1, BIT1+BIT2, BIT2+BIT3, BIT3+BIT0};
int stepPos = 0;

void wait(volatile unsigned int i){
	do i--;
	while(i > 0);
}

void turn(unsigned int cw){
	P2OUT &= ~(BIT0+BIT1+BIT2+BIT3);
	if(cw==1)
	  stepPos= stepPos==3 ? 0 : stepPos+1;
	else
	  stepPos= stepPos==0 ? 3 : stepPos-1;
	P2OUT |= steps[stepPos];
}

void setup() {
    ctr=0;
}

void main(void) {
    setup();
    WDTCTL = WDTPW + WDTHOLD;             // Stop watchdog timer
    P2DIR |= BIT0 + BIT1 + BIT2 + BIT3;
    P2OUT &= ~(BIT0 + BIT1 + BIT2 + BIT3);
    P1DIR &= ~(BIT3+BIT4+BIT5);

    for(;;){
    	if(P1IN&BIT4){
    	}
    	else if(P1IN&BIT3){
    		turn(1);
    		wait(10000);
    	}
    	else if(P1IN&BIT5){
    		turn(0);
    		wait(10000);
    	}
    }
}
