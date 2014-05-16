#include <msp430.h> 
#include <stdlib.h>

#define TXD BIT2

void wait( volatile int i ){//stupid wait function
	do {i--;}
	while (i>0);
}

long voltage;
void uart_init( void ) {
    /**
     * 1. Set UCSWRST
     * 2. Initialize all USCI registers
     * 3. Configure ports
     * 4. Clear UCSWRST
     * 5. Enable interrupts via UCxRXIE and/or UCxTXIE (optional)
     */

    // (1) Set state machine to the reset state
    UCA0CTL1 = UCSWRST;

    // (2) Initialize USCI registers
    UCA0CTL1 |= UCSSEL_2;               // CLK = SMCLK
    UCA0BR0 = 104;                      // 1MHz/9600 = 104
    UCA0BR1 = 0x00;                     //
    UCA0MCTL = UCBRS0;                  // Modulation UCBRSx = 1

    // (3) Configure ports
    P1SEL = TXD;
    P1SEL2= TXD;

    // (4) Clear UCSWRST flag
    UCA0CTL1 &= ~UCSWRST;               // **Initialize USCI state machine**

    // (5) Enable interrupts
}

void uart_send_byte( unsigned char data ) {
    // Wait until USCI_A0 TX buffer is ready
    while (!(IFG2&UCA0TXIFG)) ;

    // Send the byte
    UCA0TXBUF = data;
}

void main(void)
{
    WDTCTL = WDTPW + WDTHOLD;           // Stop Watchdog timer

    // The UART settings used depend on a good 1MHz clock
    BCSCTL1 = CALBC1_1MHZ;
    DCOCTL = CALDCO_1MHZ;

    uart_init();

    ADC10CTL0 = SREF_0 | ADC10SHT_2 | ADC10ON;  // Vcc reference, 16 clocks, on
    ADC10CTL1 = INCH_5 | SHS_0 | ADC10DIV_0 | ADC10SSEL_0 | CONSEQ_0;
    ADC10AE0 = BIT5;    // enable analog input on A5
    ADC10CTL0 |= ENC;   // enable conversions

    __bis_SR_register( GIE );

    while( 1 ) {
        ADC10CTL0 |= ADC10SC;                   // trigger the conversion
        while( ADC10CTL1 & ADC10BUSY ) ;        // loop until done
        voltage = ADC10MEM * 3.55;           // convert to range 0-3600
    	uart_send_byte ( voltage/1000 + 48 ); //thousands
    	wait(1000);
    	uart_send_byte ( voltage/100 - (voltage/1000)*10 + 48);//hundreds
    	wait(1000);
    	uart_send_byte (48);//tens
    	wait(1000);
    	uart_send_byte( 95 );           // carriage return
    }
}

