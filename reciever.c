/***************************************************************************************
 * P1.0|>D1
 * P1.1|<UART
 * P1.2|<UART
 * P1.3|>D2
 * P1.4|>D3    P1.7|>TL
 * P1.5|>D4    P1.6|>TR
 * ----------------------
 * P2.0|>T    P2.5|>B -> BL
 * P2.1|>BL   P2.4|>D
 * P2.2|>BR   P2.3|>M
 *  Built with Code Composer Studio v5
 **************************************************************************************/

#include <msp430.h>

#include <stdlib.h>

char rcvd[] = {'0','0','0'};
int j = 0; // Keeps track of received bits

//UART
#define RXD BIT1

//Digit pins MUST be on port 1!
#define DIGIT1 BIT0
#define DIGIT2 BIT3
#define DIGIT3 BIT4
#define DIGIT4 BIT5
#define DIGITS (DIGIT1 | DIGIT2 | DIGIT3 | DIGIT4)

//Caution, this is useful shorthand, but you must still use the proper port!
#define T BIT0
#define TL BIT7
#define D BIT4
#define BR BIT2
#define M BIT3
#define TR BIT6
#define BL BIT5
#define B BIT1

#define Dig0p1 (TR | TL)
#define Dig1p1 TR
#define Dig2p1 TR
#define Dig3p1 TR
#define Dig4p1 (TL | TR)
#define Dig5p1 TL
#define Dig6p1 TL
#define Dig7p1 TR
#define Dig8p1 (TR | TL)
#define Dig9p1 (TR | TL)

#define Dig0p2 (T | BL | BR | B)
#define Dig1p2 BR
#define Dig2p2 (T | M | BL | B)
#define Dig3p2 (T | M | BR | B)
#define Dig4p2 (M | BR)
#define Dig5p2 (T | M | BR | B)
#define Dig6p2 (T | M | BL | B | BR)
#define Dig7p2 (T | BR)
#define Dig8p2 (T | M | BR | BL | B)
#define Dig9p2 (M | BR | B | T)

unsigned short numbers1[10] = { Dig0p1, Dig1p1, Dig2p1, Dig3p1, Dig4p1, Dig5p1, Dig6p1, Dig7p1, Dig8p1, Dig9p1 };
unsigned short numbers2[10] = { Dig0p2, Dig1p2, Dig2p2, Dig3p2, Dig4p2, Dig5p2, Dig6p2, Dig7p2, Dig8p2, Dig9p2 };

void writeDigit(unsigned short dig, int num){
	P1OUT &= ~DIGITS; // Deselect last digit
	P1OUT |= numbers1[8]; // Turns off all used segments
	P2OUT |= numbers2[8];
	P1OUT &= ~numbers1[num];
	P2OUT &= ~numbers2[num];
	P1OUT |= dig; //Output
}

char raw_digits[3];
int digit=0;

void wait(volatile unsigned int i){
	do i--;
	while(i > 0);
}

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;		// Stop watchdog timer
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;
	P1SEL |= RXD;
	P1SEL2 |= RXD;
	UCA0CTL1 = UCSWRST;
	UCA0CTL1 |= UCSSEL_2;               // CLK = SMCLK
    UCA0BR0 = 104;                      // 1MHz/9600 = 104
    UCA0BR1 = 0x00;                     //
    UCA0MCTL = UCBRS0;                  // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST;               // **Initialize USCI state machine**
    UC0IE |= UCA0RXIE; // Enable USCI_A0 RX interrupt
    __bis_SR_register(GIE);
	P1DIR |= numbers1[8] | DIGITS;
	P2DIR |= numbers2[8];
	P2OUT |= D;
	for(;;) {
		writeDigit(DIGIT3, digit/10000);
		wait(100);
		writeDigit(DIGIT2, (digit/1000)%10);
		wait(100);
		writeDigit(DIGIT1, (digit/100)%10);
		wait(100);
	}
}

#pragma vector=USCIAB0RX_VECTOR
__interrupt void USCI0RX_ISR(void)
{
	char txbyte = UCA0RXBUF;
	if (txbyte == '_'){
		j=0;
		digit=atoi(&rcvd[0]);
	}
	else{
		if(txbyte>47 && txbyte<58){	// Make sure you got numbers
			rcvd[j]=txbyte;
			if(j<2) j++;
		}
	}
}
