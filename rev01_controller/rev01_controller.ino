/*
* This is code to run the Teensy 3.2 on rev01 control board for 4runner heated seats
*
* PIN ASSIGNMENTS:
* 4 - Output - Right Switch Backlight Illum 
* 14/A0 - Input - Right Switch potentiometer reading
* 22/A8 - Output - Right heater output
* 
* 3 - Output - Left Switch Backlight illum
* 15/A1 - Input - Left swtich potentiometer reading
* 23/A9 - Output - Left heater output
* 
* 16/A2 - Input - +12 volt reading
* 17/A3 - Input - +12v when car lights are on
*
 */

// Pin setup
#define RightSWBacklightPin 4
#define LeftSWBacklightPin 3

#define RightSWPotPin 14
#define LeftSWPotPin 15

#define RightHeatPin 22
#define LeftHeatPin 23

#define DC12VReadPin 16
#define LightsReadPin 17

//Global Variables

int DebugMode = 1;
unsigned long previousMillis = 0;

//Global IO variables

int RightSWBacklightOut = 0;
int LeftSWBacklightOut = 0;

int RightHeatOut = 0;
int LeftHeatOut = 0;

int RightSWReading = 0;
int LeftSWReading = 0;

int DC12VReading = 0;
int LightsReading = 0;

int HeatOutLevelArray[] = {0,50,100,150,200,255}; // the variable to make setting non-linear heating settings easy (0-255)

// math variables
int ScaledRightSWReading = 0;
int ScaledLeftSWReading = 0;

// Sleep and timing variables
bool OutputInhibit = 1; //inhibit all outputs to save battery
int EngineOffVoltage = 0;  //at what voltage is the battery not being charged

void setup() {
  pinMode(RightSWBacklightPin, OUTPUT);
  pinMode(LeftSWBacklightPin, OUTPUT);
  pinMode(RightSWPotPin, INPUT);
  pinMode(LeftSWPotPin, INPUT);
  pinMode(RightHeatPin, OUTPUT);
  pinMode(LeftHeatPin, OUTPUT);
  pinMode(DC12VReadPin, INPUT);
  pinMode(LightsReadPin, INPUT);
  analogWriteFrequency(RightHeatPin,60); //this setting allows the mosfet to reach full saturation
  analogWriteFrequency(LeftHeatPin,60);
  delay (100);
  if (DebugMode == 1){ //disable the serial and debug messages if debug is not set
  Serial.begin(115200);
    delay(50);
    Serial.println("Setup Complete...");
  }
  
}

void loop() {
  ReadInputs (); //read the digital inputs
  WriteToHeaters (); //write the calculated values to the heater Mosfets
  ReadAndSetBacklights (); //as the function name says
  PlotReadings ();
  Serial.println("In Loop....");
  delay(100);
}

void ReadInputs () { //function to read all the inputs to the system
  RightSWReading = analogRead(RightSWPotPin);
  LeftSWReading = analogRead(LeftSWPotPin);
//  DC12VReading = analogRead(DC12VReadPin);
//  if (DC12VReading > EngineOffVoltage){
//    OutputInhibit = 0;
//  }else{
//    OutputInhibit = 1;
//  }
  ScaledLeftSWReading = (((LeftSWReading*100)/DC12VReading)*10); // Scale the read pot value as a percentage of the read main voltage
    if (ScaledLeftSWReading <= 100) { //if the pot is off (or as near as possible to off), set the heaters off
    LeftHeatOut = HeatOutLevelArray[0];
  }else if (585 >= ScaledLeftSWReading && ScaledLeftSWReading >= 535){
    LeftHeatOut = HeatOutLevelArray[1];
  }else if (685 >= ScaledLeftSWReading && ScaledLeftSWReading >= 586){
    LeftHeatOut = HeatOutLevelArray[2];
  }else if (820 >= ScaledLeftSWReading && ScaledLeftSWReading >= 686){
    LeftHeatOut = HeatOutLevelArray[3];
  }else if (915 >= ScaledLeftSWReading && ScaledLeftSWReading >= 821){
    LeftHeatOut = HeatOutLevelArray[4];   
  }else if (ScaledLeftSWReading >= 916){
    LeftHeatOut = HeatOutLevelArray[5];
    }
  ScaledRightSWReading = (((RightSWReading*100)/DC12VReading)*10); // Scale the read pot value as a percentage of the read main voltage
    if (ScaledRightSWReading <= 100) { //if the pot is off (or as near as possible to off), set the heaters off
    RightHeatOut = HeatOutLevelArray[0];
  }else if (585 >= ScaledRightSWReading && ScaledRightSWReading >= 535){
    RightHeatOut = HeatOutLevelArray[1];
  }else if (685 >= ScaledRightSWReading && ScaledRightSWReading >= 586){
    RightHeatOut = HeatOutLevelArray[2];
  }else if (820 >= ScaledRightSWReading && ScaledRightSWReading >= 686){
    RightHeatOut = HeatOutLevelArray[3];
  }else if (915 >= ScaledRightSWReading && ScaledRightSWReading >= 821){
    RightHeatOut = HeatOutLevelArray[4];   
  }else if (ScaledRightSWReading >= 916){
    RightHeatOut = HeatOutLevelArray[5];
    }
}

void WriteToHeaters () {
  if (OutputInhibit == 0){
    analogWrite(LeftHeatPin, RightHeatOut);
    analogWrite(RightHeatPin, RightHeatOut);
  }else{
    analogWrite(LeftHeatPin, 0);
    analogWrite(RightHeatPin, 0);   
  }
}

void ReadAndSetBacklights (){
  if (OutputInhibit == 0){
    LightsReading = analogRead(LightsReadPin); 
    if (LightsReading > 195) {   //if our voltage reading is high enough we consider it fully on, set the backlights to full
      RightSWBacklightOut = 255;
      LeftSWBacklightOut = 255;
    } else {                     //if they're not full bright, start dimming them
    RightSWBacklightOut = map(LightsReading, 0, 195, 0, 255);
    LeftSWBacklightOut = RightSWBacklightOut;
    }
    analogWrite(RightSWBacklightPin, RightSWBacklightOut);  //write the values to the outputs
    analogWrite(LeftSWBacklightPin, LeftSWBacklightOut);
  }else{
    analogWrite(RightSWBacklightPin, 0);
    analogWrite(LeftSWBacklightPin, 0);    
  }
}

void PlotReadings () {
  if (DebugMode == 1){
  //Serial.print(LeftSWReading);
  //Serial.print(" ");
  Serial.print(DC12VReading);
  Serial.print(" ");
  //Serial.print(ScaledLeftSWReading);
  //Serial.print(" ");
  //Serial.print(LeftHeatOut);
  //Serial.print(" ");
  //Serial.print(RightHeatOut);
  //Serial.print(" ");
  //Serial.print(LightsReading);
  //Serial.print(" ");
  Serial.print(RightSWReading);
  Serial.print(" ");
  Serial.print(ScaledRightSWReading);
  Serial.print(" ");
  Serial.println(RightHeatOut);
  //Serial.print(" ");
  //Serial.println(LightsReading);
  }
  }
