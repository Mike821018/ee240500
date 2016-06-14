/*
  Blank Simple Project.c
  http://learn.parallax.com/propeller-c-tutorials 
*/
#include "simpletools.h"                      // Include simpletools header
#include "servo.h"                            // Include servo header
#include "ping.h"                             // Include ping header
#include "servodiffdrive.h"
#include "fdserial.h"


int cmDist;
int TRC;
int WallSide = 0;
void LightSensor();
void DistSensor();
char char2lowercase (char c);
void Turn(int angle, int Direct);
void Rotate(int angle);
void blink1();
void PlayMusic();
void PlayMyTone(int pin,int duration, int freq, int duty);
fdserial *xbee;
int DO = 22, CLK = 23, DI = 24, CS = 25;      // SD card pins on Propeller BOE

#define C6 1047
#define D6b 1109
#define D6 1175
#define E6b 1245
#define E6 1319
#define F6 1397
#define G6b 1480
#define G6 1568
#define A6b 1661
#define A6 1760
#define B6b 1865
#define B6 1976
#define C7 2093
#define D7b 2218
#define D7 2349
#define E7b 2489
#define E7 2637
#define F7 2794
#define G7b 2960
#define G7 3136
#define A7b 3322
#define A7 3520
#define B7b 3709
#define B7 3951
#define C8 4186
#define PIN_SOUND       26 // pin for the speaker, default=26
#define PIN_XBEE_RX      0
#define PIN_XBEE_TX      1
#define PIN_SERVO_LEFT  16
#define PIN_SERVO_RIGHT 17

int DefaultMusicFreq[] = {E7, E7b, E7, E7b, E7, B6, E7b, C7, A6,
                          E6, A6, B6, E6, A6b, B6, C7,
                          E6, E7, E7b, E7, E7b, E7, B6, E7b, C7, A6,
                          E6, A6, B6, E6, C7, B6, A6};
int DefaultMusicDura[] = {1, 1, 1, 1, 1, 1, 1, 1, 4,
                          1, 1, 3, 1, 1, 1, 3,
                          1, 1, 1, 1, 1, 1, 1, 1, 1, 4,
                          1, 1, 3, 1, 1, 1, 4};

int main()
{
  xbee = fdserial_open(PIN_XBEE_RX, PIN_XBEE_TX, 0, 9600);
  drive_pins(PIN_SERVO_LEFT, PIN_SERVO_RIGHT);
  
  pause(1000);
  putchar(CLS);
  fdserial_rxFlush(xbee);
  set_pause_dt(CLKFREQ/1000000);
  cog_run(LightSensor, 128);
  cog_run(DistSensor, 128);
  cog_run(blink1, 128);
}  

void LightSensor()                                    // main function
{
  while(1)                                    // Endless loop
  {
    high(5);                                  // Set P5 high
    pause(1000);                                 // Wait for circuit to charge
    TRC = rc_time(5, 1);                    // Measure decay time on P5
    pause(100000);                               // Wait 1/10th of a second
  }
}

void DistSensor()                                    // main function
{
  while(1)                                    // Repeat indefinitely
  {
    cmDist = ping_cm(18);                 // Get cm distance from Ping)))
    pause(100000);
  }
}

void PlayMusic()
{
  while(1)
  {
    if(WallSide!=0)
    {
      for(int i = 0; i < 33; i++)
      {
        freqout(4, 300000*DefaultMusicDura[i], DefaultMusicFreq[i]);                     // pin, duration, frequency
        pause(50000);
      }
    }  
  }
}  

void PlayMyTone(int pin,int duration, int freq, int duty)
{
  int dt = duration*CLKFREQ/1000000;    // Pick a timeout, 1/2 a second in this case
  int t = CNT;           // Mark current time by storing in variable
  while(CNT - t < dt)    // Repeat until timeout
  {
    high(pin);
    pause(10000*duty/freq);
    low(pin);
    pause(10000*(100-duty)/freq);
  }
}  

void Turn(int angle, int Direct)              //Direct 0:Right 1:Left
{
  int count = 0;
  int prev, cur;
  
  servo_speed(16, 50);
  servo_speed(17, -50);
    
  count = 0;
  if(Direct==1) prev = input(15);
  else prev = input(14);
  
  while(count<22)
  {
    if(Direct==1) cur = input(15);
    else cur = input(14);
    while(cur == prev)
    {
      if(Direct==1) cur = input(15);
      else cur = input(14);
    }
    prev=cur;
    count++;
  }
  
  if(Direct==1)
  {
    servo_speed(16, 0);
    servo_speed(17, -50);
  }    
  else
  {
    servo_speed(16, 50);
    servo_speed(17, 0);
  }    
  
  count = 0;
  if(Direct==1) prev = input(15);
  else prev = input(14);
  
  while(count<48*angle/90)
  {
    if(Direct==1) cur = input(15);
    else cur = input(14);
    while(cur == prev)
    {
      if(Direct==1) cur = input(15);
      else cur = input(14);
    }
    prev=cur;
    count++;
  }
  
  servo_speed(16, 50);
  servo_speed(17, -50);
    
  count = 0;
  if(Direct==1) prev = input(15);
  else prev = input(14);
  
  while(count<33)
  {
    if(Direct==1) cur = input(15);
    else cur = input(14);
    while(cur == prev)
    {
      if(Direct==1) cur = input(15);
      else cur = input(14);
    }
    prev=cur;
    count++;
  }
  
  servo_disable(16);
  servo_disable(17);
}

void Rotate(int angle)                                    // main function
{
  servo_angle(19, angle);                   // Display array element & value
  pause(1000000);                                       // 1/2 second pause
  servo_stop();                               // Stop servo process
}

void blink1()
{
  int mode = 1;
  while(1)
  {
    int ch = fdserial_rxChar(xbee);
    ch = char2lowercase(ch);
    fdserial_rxFlush(xbee);
    switch ((char) ch) {
      case '1': mode=1; break;
      case '2': mode=2; break;
      default: break;
    }      
    //mode=2;
    if(mode==1)
    {
      //PlayMyTone(4, 300000, cmDist*150, 50-TRC/100);                     // pin, duration, frequency, duty
      freqout(4, 300000, cmDist*150);                     // pin, duration, frequency
      pause(50000);
    }
    else if(mode==2)
    {    
      int count = 0;
      int NearWall = 10;
      int FarWall = 20;
      int Empty = 30;
      WallSide = 0;                 //0:No wall 1:The wall is at RHS, 2:The wall is at LHS
      pause(500000);
      Rotate(1800);
      pause(1000000);
      if(cmDist>Empty)
      {
        Rotate(0);
        pause(1000000);
        if(cmDist<Empty)
        {
          WallSide = 1;
          cog_run(PlayMusic, 128);
        }        
      }  
      else
      {
        WallSide = 2;
        cog_run(PlayMusic, 128);
      }          
      while(WallSide!=0)
      {
        int slowR = 0;
        int slowL = 0;
        if(WallSide==1&&cmDist<=NearWall||WallSide==2&&cmDist>=FarWall) slowR = 15;
        if(WallSide==2&&cmDist<=NearWall||WallSide==1&&cmDist>=FarWall) slowL = 15;
        servo_speed(16, 50-slowR);           // P14 servo to full speed counterclockwise
        servo_speed(17, -50+slowL);          // P15 servo to full speed clockwise
        if(cmDist>=Empty)
        {
          if(WallSide==1)
          {
            pause(500000);
            Turn(90,0);
            count++;
            if(count==2) 
            {
              servo_stop();
              Rotate(1800);
              count=0;
              pause(100000);
              if(cmDist>=Empty) WallSide=0;
              else WallSide=2;
            }              
          }            
          else if(WallSide==2)
          {
            pause(500000);
            Turn(90,1);
            count++;
            if(count==2) 
            {
              servo_stop();
              Rotate(0);
              count=0;
              if(cmDist>=Empty) WallSide=0;
              else WallSide=1;
            }       
          } 
          else break;           
        }
        /*else if((cmDist<=NearWall && WallSide==1)||(FarWall<=cmDist && cmDist<=Empty && WallSide==2))
        {
          servo_speed(16, 0);
          servo_speed(17, -10); 
        }       
        else if((cmDist<=NearWall && WallSide==2)||(FarWall<=cmDist && cmDist<=Empty && WallSide==1))
        {
          servo_speed(16, -10);
          servo_speed(17, 0); 
        }       */
        else continue;  
      }        
    }   
    servo_stop(); 
    if(mode==2) 
    {
      mode=0;
      Rotate(900);
    }     
    pause(100000);
    fdserial_txChar(xbee, ch);
    fdserial_txFlush(xbee);
  }    
}

char char2lowercase (char c) {
	return 'A'<=c && c<='Z' ? c+32 : c;
}
