
//MVALVE_pin = 7;  // output for magnetic valve switching transistor and relay
int cpin=5;  // direct connected SSR relay for ULKA PUMP
int DC = 0;  // % power of PUMP  (0-10)  = 0-100% in 10 steps
boolean State;
volatile int Pulse;  // zero crossing in 200ms interval for 1 diode optocoupler
unsigned long Now; // now time
unsigned long StartTime; // start time of brewing 
unsigned long PreTime1, PreTime2;  // time sectors profile
int var; // variable for dialing profiles
int dutyP1, dutyP2;  // DC sectors profile
int DCcycle[10][10] = {
        {0,0,0,0,0,0,0,0,0,0},    //DC 0
        {1,0,0,0,0,0,0,0,0,0},    //DC 10%
        {1,0,0,0,0,1,0,0,0,0},    //DC 20    
        {1,0,0,1,0,0,1,0,0,0},    //DC 30
        {1,0,1,0,1,0,0,1,0,0},    //DC 40
        {1,0,1,0,1,0,1,0,1,0},    //DC 50
        {1,1,0,1,1,0,1,1,0,0},    //DC 60
        {1,1,1,1,1,0,1,0,1,0},    //DC 70
        {1,1,1,1,1,1,1,0,1,0},    //DC 80
        {1,1,1,1,1,1,1,1,1,0}     //DC 90%    100% : PUMP full on without this pattern
        }; 

void ZERO_cross() //  event by zero-crossing
{          
    Pulse++;  // with 1 diode rectifier counting only half wave  = 20ms for 50Hz ( rising-event)),  with 2 diode rectifier counting both waves  = 10ms for 50Hz
    if (Pulse > 10)
          Pulse = 1;   
    pwm_pump(DC);
    DC = profile(Now);      // profile - DC cycle for PUMP in time                                                                                    
}

void pump_on()  // switching PUMP on
{
  if (!State)
        { digitalWrite(cpin, HIGH); State = true; }      
}   

void pump_off()  // switching PUMP off
{
  if (State)
        { digitalWrite(cpin, LOW); State = false; }        
}  

void pwm_pump(int dc)  ////  PUMP power adjustment
{                          
        if (dc == 10)
            pump_on();  
        else if (dc > 0  && Pulse > 0)
           { 
            if (DCcycle[dc][Pulse-1] == 1)
                   pump_on();                                                    
             else
                   pump_off(); 
           }         
        else
           pump_off();     
}

int profile(unsigned long now)  //// DC cycle in time and profiles   DC is dependent on hardware - pump, OPV pressure, basket type... ***
{                
            // example of use of dc
            int dc = 0; unsigned long tx = now-StartTime;  
            if  ( tx <= PreTime1 )                     
              dc = dutyP1;
            else if (tx > PreTime1 && tx <= PreTime2 )        
              dc = dutyP2;      
            else if (tx > PreTime2 )      
              dc = 10;                           
            //......                                                  
            return dc;       
}

void profile_list()  // profile of DC ***
{                
       switch (var)
       {
         case 1:   //     P1 standard profile 
          { PreTime1 = 6000; PreTime2 = 10000; dutyP1 = 3; dutyP2 = 2; }                                       
          break;    
         case 2:   //     P2 profile    
          //...
          break;                                
         // .....                           
       }  
}
void startBrewing()  
{                           
       Pulse =0; StartTime = millis(); digitalWrite(7, HIGH); // MVALVE on      
       attachInterrupt(1,ZERO_cross, FALLING);                                     
}

void stopBrewing()  
{     
       pump_off();
       detachInterrupt(1); digitalWrite(7, LOW);     // MVALVE off       
}

void setup()
{       
  pinMode(7, OUTPUT); digitalWrite(7, LOW);   
  pinMode(cpin, OUTPUT); digitalWrite(cpin, LOW); 
}

void loop()  // main program loop
{  
  Now = millis();
  //if ...   // waiting for user activity
      //start ....
      profile_list();       
      startBrewing();                                             
  //if ....
      //stop .....
      stopBrewing();                                   
}













