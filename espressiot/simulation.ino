//
// ESPressIoT Controller for Espresso Machines
// 2016 by Roman Schmitz
//
// Simple Simulation Code
//

#ifdef SIMULATION_MODE

#define SIM_TIME 100
#define SIM_T_START 20
#define SIM_T_LOSS 2.9e-2
#define SIM_T_HEAT 0.7787

float heaterSavedState = 0.0;
float curTemp = 0.0;
unsigned long lastSensTime = 0;

void setupSensor() {
  curTemp = SIM_T_START;
}

void setupHeater() {
  heatcycles = 0;
  pinMode(HEAT_RELAY_PIN , OUTPUT);
}

void updateTempSensor() {
  if (abs(time_now - lastSensTime) >= SIM_TIME) {
      lastSensTime=time_now;
      curTemp = (curTemp<SIM_T_START)?(SIM_T_START):(curTemp+(heaterSavedState*SIM_T_HEAT*1e-3)-SIM_T_LOSS);
  }  
}

#ifndef ARDUINO_UNO
void updateHeater() {
  heatCurrentTime = time_now;
  if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) {
    heaterSavedState = getHeatCycles();
    _turnHeatElementOnOff(0);
    heatLastTime = heatCurrentTime;   
  }
}
#endif

void updateHeater() {
  boolean h;
  heatCurrentTime = time_now;
  if(heatCurrentTime - heatLastTime >= 1000 or heatLastTime > heatCurrentTime) {
    // begin cycle
    heaterSavedState = getHeatCycles();
    _turnHeatElementOnOff(1);  // 
    heatLastTime = heatCurrentTime;   
  } 
  if (heatCurrentTime - heatLastTime >= heatcycles) {
    _turnHeatElementOnOff(0);
  }
}

float getTemp() {
  return curTemp+((float)random(-10,10))/100;;
}

#endif

