#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <inttypes.h>



volatile int T1capture = 0;
volatile int period = 0;
volatile int lastT1capture = 0;


ISR (TIMER1_CAPT_vect)
{
    // read timer1 input capture register
    T1capture = (signed int)ICR1 ; 
    // compute time between captures
    period =  T1capture - lastT1capture;
    lastT1capture = T1capture ;
}

void setup()
{
  Serial.begin(9600);
  //Serial.println(0);
    //set up timer1 for full speed and
  //capture an edge on analog comparator pin B.3 
  // Set capture to positive edge, full counting rate
  TCCR1B = (1<<ICES1) + 1; 
  TCCR1A = 0 ;
  // Turn on timer1 interrupt-on-capture
  TIMSK1 = (1<<ICIE1) ;
  
  
  
}

void loop()
{
  //Serial.println(0);
   Serial.println(period);
   delay(300);
}
