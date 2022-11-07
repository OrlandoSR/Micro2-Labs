#include <msp430.h> 


/**
 * main.c
 */

/* Serial Communication Settings
 *
 *  Baud Rate = 9600
 *  Data Bits = 8
 *  Parity Bits = None
 *  Flow Control = None
 *
 *
 * MCU Baud Rate Settings (Family Guide p.952)
 *  BRCLK Frequency | Baud Rate | UCBRx | UCBRSx | USBRFx
 *      32,768          9600        3       3       0
 *    1,000,000         9600        104     1       0
 *
 */

/*
 * To initialize or reconfigure USCI module
 *  Set UCSWRST (UCAxCTL1 |= UCSWRST)
 *  Initialize all USCI registers with UCSWRST = 1
 *  Configure all ports
 *  Clear UCSWRST via software (UCAxCTL1 &=~ UCSWRST)
 *  Enable interrupts if needed
 *
 */

/*
 * USCI Module used is USCI_A2
 *
 * Use Tx = UCA2TXD (P9.4) and Rx = UCA2RXD (P9.5)
 *
 */


/*       Flow Chart
 *
 *
 *          START
 *            |
 *            |<------.
 *            |       |
 *          UART     F|
 *         Empty?_____|
 *            |
 *            |T
 *            |
 *    TX BUFF = Character
 *            |
 *            |
 *           END
 *
 *
 */

/*  EQUATIONS
 *
 *                           fclk
 *     Baud Rate =  -------------------------
 *                  Prescaler * (divider + 1)
 */


int main(void)
{
    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer

/*  UART Configuration
    *   Configure baud control registers
    *   Select clock source, specify prescaler and divider
    *   Specify Asynchronous mode in control register
    *   Enable transmitter and global USART module
    *   Configure Tx and Rx pins selected to work with UART module
*/

    //USCI control registers

    UCA2CTL1 |= UCSWRST;    //Enable modificattion to all USCI control registers

    //UCA2CTL0 (Control Register 0)
    UCA2CTL0 &=~ UCSYNC;    //Set asynchronous mode
    UCA2CTL0 |= UCMODE_0;   //Set UART mode
    UCA2CTL0 &=~ (UCPAR | UCMSB | UC7BIT | UCSPB); //Configure parity, endianness, data size and stop bits

    //UCA2CTL1 (Control Register 1)
    UCA2CTL1 |= UCSSEL__SMCLK;    //BRCLK selected is SMCLK 1MHz

    //UCA2BRW   (Baud Rate Select [use table above] )
    UCA2BRW = 104;            //Set Baud Rate to 9600 / This edits the 16-bit word insted of two 8-bit registers

    //UCA2IE    (Interrupt Enable Register)
//    UCA2IE = UCTXIE;       //Enable Tx interrupt

  UCA2CTL1 &=~ UCSWRST;   //Disable modificattion to all USCI control registers

  //Pin Configuration
  P9SEL |= (BIT4 | BIT5);

//    Not sure
    P9DIR |= BIT4;
    P9DIR &=~ BIT5;



    while(1){
      if(UCA2IFG & BIT1){   //Check if Tx buffer is empty
//          __delay_cycles(1000000);
          UCA2TXBUF = 'A';  //Send data to tx buffer
      }
    }
}
