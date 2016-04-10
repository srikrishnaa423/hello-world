#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


RF24 radio(9,10);                                             //initializing setup data
const uint64_t pipes[2] = { 0x000000002LL, 0x0000000003LL };
typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_ping_out;

unsigned char A[7][9] = {
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9},
  {1,2,3,4,5,6,7,8,9}};
  
  unsigned char temp1[32];       
  unsigned char temp2[32];






void setup()
{
  Serial.begin(57600);               //prints inital data
  printf_begin();
  printf("\n\rRF24/examples/GettingStarted/\n\r");
  printf("ROLE: %s\n\r",role_friendly_name[role]);
  printf("*** PRESS 'T' to begin transmitting to the other node\n\r");
  
  
  
  radio.begin();                 //sets up radio
  radio.setRetries(15,15);
  radio.setAutoAck(true);
  radio.setChannel(0x50);
  radio.setPALevel(RF24_PA_MIN);
  radio.setDataRate(RF24_250KBPS);
  
  
  
   if ( role == role_ping_out )             //sets reading and writing pipes
  {
    radio.openWritingPipe(pipes[0]);
    radio.openReadingPipe(1,pipes[1]);
  }
  else
  {
    radio.openWritingPipe(pipes[1]);
    radio.openReadingPipe(1,pipes[0]);
  }
  
  
  radio.startListening();             //open to receiving
  radio.printDetails();
  
  ImplementProtocol();             //sets up the two payloads to send

  
  
}

void loop(void)
{   
  int i = 0;

  if(role == role_ping_out )
  {
    bool fail_1 = true;
    bool fail_2 = true;
    
    while(fail_1)
    {
      fail_1= TransmitData(temp1);
    }
    
     while(fail_2)
    {
//      for(i = 0; i < 32; i++)
//      {
//        printf("%u ",temp2[i]);
//      }
//      printf("\n");
      fail_2= TransmitData(temp2);
    }
  }


}











bool TransmitData(unsigned char data[32])
{
  radio.stopListening();
  
   printf("Now sending  data...\n");                    //sending data
   bool ok = radio.write( data, sizeof(unsigned char)*32 );

   if (ok)                                                //printing if data is sent
     printf("Sent ok...\n");
   else
     printf("Sent failed.\n\r");
     
     
   radio.startListening();                              //set to reveice confirmation
  
  
    unsigned long started_waiting_at = millis();           // confirm sent message 
    bool timeout = false;
    while ( ! radio.available() && !timeout )
    {  
      if (millis() - started_waiting_at > 200 )
        timeout = true; 
    }
  
   if ( timeout )    
    {
      printf("Failed, response timed out. Not received confirmation. \n\r");
    }
    else
    {
      unsigned char got_data[32];
      radio.read( got_data, sizeof(unsigned char)*32);
      printf("Confirmed sent \n");
    }
     return timeout;
}






void ImplementProtocol()
{
  for(int i = 0; i < 63; i++)     //converting the data matrix into 2 payloads with protocol impplemented
  {
    int x = i/9;
    int y = i % 9;
    temp2[0] = 10;
    if(i < 32)
    {
      temp1[i] = A[x][y];
    }
    else
    {
        temp2[i-31] = A[x][y];
    }
  }
}

