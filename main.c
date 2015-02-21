#include <msp430.h>
#include "RF_a.h"
#include "RF_a.c"

#define LED0 BIT2
#define zerocross1 BIT3
#define zerocross2 BIT4
#define TX BIT3
#define temp1 INCH_0
#define temp2 INCH_5
#define triac BIT1

#define DOWN 0
#define UP 1
#define FALSE 0
#define TRUE 1
#define BRIGHTNESS_MIN 85
#define BRIGHTNESS_MAX 5


int direction = UP;
unsigned int RFsendcount =5000;
int temperature1 =0;
int temperature2 =0;
int active = FALSE;
int AConCount =0;
int ledcount= 1000;
int brightness =0;
int brightnessTEMP =0;
int triacCount =0;
int tempcount =0;
int loopcount =0;
int powerdown = FALSE;

const unsigned char Aux2OFF[] = {
  0,1,1,0,1,0,0,0,0,0,0,1,0,0,0,1}; // door closed

int timeout =0;

int main(void) {
  WDTCTL = WDTPW | WDTHOLD;		// Stop watchdog timer
  P1DIR |= (LED0 + triac);
  P1OUT &= ~(LED0 + triac);
  P2DIR |= (TX);
  P2OUT &= ~(TX);

  unsigned int analogRead(unsigned int pin);
  P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);
    P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);
    P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);
    P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);
    P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);
    P1OUT |= LED0;
    __delay_cycles(200000);
    P1OUT &= ~LED0;
    __delay_cycles(200000);

    P1IE |= zerocross1; // P1.3 interrupt enabled
    P1IES &= ~zerocross1; // P1.3 interrupt enabled
    P1IFG &= ~zerocross1; // P1.3 IFG cleared
    P1IE &= ~zerocross2; // P1.3 interrupt enabled
    P1IES |= zerocross2; // P1.3 interrupt enabled
    P1IFG &= ~zerocross2; // P1.3 IFG cleared

  TA0CCTL0 |= CCIE;                             // CCR0 interrupt enabled
  TA0CTL = TASSEL_2 + MC_1 + ID_2;           // SMCLK/8, upmode
  TA0CCR0 =  20;

  __enable_interrupt(); // enable all interrupts                                   // Enable CPU interrupts


  while(1)
  {
    __delay_cycles(15000);
    if(brightness > BRIGHTNESS_MIN)
    {
      direction =DOWN;
      temperature1 =  analogRead(temp1);
      temperature2 =  analogRead(temp2);
      if(temperature1 < 340 || temperature2 < 340)
      {
        while(1)
        {
          temperature1 =  analogRead(temp1);
          _delay_cycles(1000);
          temperature2 =  analogRead(temp2);
          _delay_cycles(1000);
          if(temperature1 < 280 || temperature2 < 280)
          {
            tempcount++;
            if(tempcount>20)
            {
              tempcount=0;
              powerdown=TRUE;
              break;

            }
          }
          else
          {
            tempcount--;
            if(tempcount<0)
            {
              tempcount=0;
              break;
            }
          }
          loopcount++;
          if(loopcount>40)
          {
            loopcount=0;
            break;
          }
        }

      }
    }
      else if (brightness <BRIGHTNESS_MAX)
      {
        direction = UP;
      }
      if(direction == UP)
      {
        brightness++;
      }
      else
      {
        brightness--;
      }

      if(powerdown==1)
      {
        __delay_cycles(100);
        //sendRF
        if(RFsendcount>400   && active == TRUE)
        {
          P1IE &= ~zerocross1; // P1.3 interrupt enabled
          TA0CCTL0 &= ~CCIE;
          P1OUT &= ~triac;
          __delay_cycles(1000);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          send_rf_conf2(TX, (unsigned char*)Aux2OFF);
          RFsendcount=0;
          P1OUT |= LED0;
          ledcount=0;
        }
      }
    if(ledcount== 100)
    {
      P1OUT &= ~LED0;
    }
    ledcount++;
    if(ledcount> 105)
    {
      ledcount=105;
    }
    RFsendcount++;
    if(RFsendcount>500)
    {
      RFsendcount =500;
    }
    AConCount++;
    if(AConCount > 20)
    {
      active = FALSE;
    }
    else
    {
      active = TRUE;
    }

  }
}


#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void)
{

  AConCount =0;
  P1OUT &= ~triac;
  brightnessTEMP =  brightness;
  triacCount=0;
  P1IFG &= ~(zerocross1 +zerocross2); // P1.3 IFG cleared
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer0_A0 (void)
{
  triacCount++;
  if(triacCount > brightnessTEMP)
  {
    P1OUT |= triac;
  }
}

unsigned int analogRead(unsigned int pin) {
  ADC10CTL0 = ADC10ON + ADC10SHT_2 + SREF_0;
  ADC10CTL1 = ADC10SSEL_0 + pin;
  if(pin==INCH_5){
    ADC10AE0 = 0x20;
  }
  else if(pin==INCH_0){
    ADC10AE0 = 0x01;
  }
  ADC10CTL0 |= ENC + ADC10SC;
  while (1) {
    if (((ADC10CTL0 & ADC10IFG)==ADC10IFG)) {
      ADC10CTL0 &= ~(ADC10IFG +ENC);
      break;
    }
  }
  return ADC10MEM;
}


