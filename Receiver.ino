#include <SPI.h>
#include "nRF24L01.h"
#include "RF24.h"
#include "printf.h"


RF24 radio(9,10);                                             //initializing setup data
const uint64_t pipes[2] = { 0x000000002LL, 0x0000000003LL };
typedef enum { role_ping_out = 1, role_pong_back } role_e;
const char* role_friendly_name[] = { "invalid", "Ping out", "Pong back"};
role_e role = role_pong_back;

unsigned char A[7][9] = {
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0,0}};               //initial values for A. 
  
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

}

void loop(void)
{
  int i =0;
  int j = 0;
  ReceiveData();  
  for(i = 0; i < 7; i++)
  {
    for(j = 0; j < 9 ; j++)
    {
      printf("%u ",A[i][j]);
    }
    printf("\n");
  }


}




void  ReceiveData()
{
  unsigned char data[32];
  if ( radio.available() )                      //to send if data is available to read
    {
      
      bool done  = false;
      while (!done)
      {
        done = radio.read(data, sizeof(unsigned char)*32);
        printf("Got payload [0] %u..\n",data[0]);
        delay(20);
      }
      
      
      radio.stopListening();                      //write back data so transmitter can confirm
      radio.write( data, sizeof(unsigned char)*32 );
      printf("Sent confirmation.\n\r");
      
      radio.startListening();                     //ready to receive data again
      ImplementProtocol(data);
    }
    
  

}



void ImplementProtocol(unsigned char data[32])
{
  if(data[0] != 10)
  {
    for(int i = 0; i < 32; i++)     //converting the data matrix into 2 payloads with protocol impplemented
    {
      int x = i/9;
      int y = i % 9;
      A[x][y] = data[i];
    }
  }
  else
  {
    for(int i = 32; i < 63; i++)     //converting the data matrix into 2 payloads with protocol impplemented
    {
      int x = i/9;
      int y = i % 9;
      A[x][y] = data[i-31];
    }
  }
}

