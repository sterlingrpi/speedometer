//calculations
//tire radius ~ 13.5 inches
//circumference = pi*2*r =~85 inches
//max speed of 35mph =~ 616inches/second
//max rps =~7.25

#include <Stepper.h>

#define reed A0//pin connected to read switch
#define STEPS 600

Stepper stepper(STEPS, 1, 0, 2, 3);

//storage variables
int reedVal;
long timer;// time between one full rotation (in ms)
float mph;
float radius = 13.5;// tire radius (in inches)
float circumference;
int inc = 0; //amount to increment the motor
int pos = 0; //current position
int posold = 0; //position from last iteration

int maxReedCounter = 50;//min time (in ms) of one rotation (for debouncing) max speed = 90mph
int reedCounter;


void setup(){
  
  reedCounter = maxReedCounter;
//  circumference = 2*3.14*radius;
  circumference = 78.23; //205/55ZR16 tire with 24.9" diameter
  pinMode(reed, INPUT);
  
  // TIMER SETUP- the timer interrupt allows precise timed measurements of the reed switch
  //for more info about configuration of arduino timers see http://arduino.cc/playground/Code/Timer1
  cli();//stop interrupts

  //set timer1 interrupt at 1kHz
  TCCR1A = 0;// set entire TCCR1A register to 0
  TCCR1B = 0;// same for TCCR1B
  TCNT1  = 0;
  // set timer count for 1khz increments
  OCR1A = 1999;// = (1/1000) / ((1/(16*10^6))*8) - 1
  // turn on CTC mode
  TCCR1B |= (1 << WGM12);
  // Set CS11 bit for 8 prescaler
  TCCR1B |= (1 << CS11);   
  // enable timer compare interrupt
  TIMSK1 |= (1 << OCIE1A);
  
  sei();//allow interrupts
  //END TIMER SETUP
  
  Serial.begin(9600);
  stepper.setSpeed(45);
  stepper.step(-600);
  stepper.step(600);
}


ISR(TIMER1_COMPA_vect) //Interrupt at freq of 1kHz to measure reed switch
{
  reedVal = !digitalRead(reed);//get val of A0. inverted because hall effect sensor is normally high
  if (reedVal)//if reed switch is closed
  {
    if (reedCounter == 0)//min time between pulses has passed
    {
      mph = (56.8*float(circumference))/float(timer);//calculate miles per hour
      timer = 0;//reset timer
      reedCounter = maxReedCounter;//reset reedCounter
    }
    else{
      if (reedCounter > 0)//don't let reedCounter go negative
      {
        reedCounter -= 1;//decrement reedCounter
      }
    }
  }
  else//if reed switch is open
  {
    if (reedCounter > 0)//don't let reedCounter go negative
    {
      reedCounter -= 1;//decrement reedCounter
    }
  }
  if (timer > 1000)
  {
    mph = 0;//if no new pulses from reed switch- tire is still, set mph to 0
  }
  else
  {
    timer += 1;//increment timer
  } 
}

void displayMPH(){
  Serial.println(pos);
}

void loop(){
  //print mph once a second
  pos = 10*mph;
  inc = pos - posold;
  posold = pos;
//  displayMPH();
  stepper.step(-inc);
//  delay(250);
}
