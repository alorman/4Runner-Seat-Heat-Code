/*
* This is code to run the Teensy 3.2 on rev00 control board for 4runner heated seats
*
* PIN ASSIGNMENTS:
* 13 - Output - Right Switch Backlight Illum 
* 14 - Input - Right Switch potentiometer reading
* 18 - Output - Right heater output
* 
* 19 - Output - Left heater output
* 20 - Output - Left Switch Backlight illum
* 23 - Input - Left swtich potentiometer reading
* 
* 21 - Input - +12 volt reading
* 22 - Input - +12v when car lights are on
*
 */

// Pin setup
#define RightSWBacklightPin 13
#define LeftSWBacklightPin 20

#define RightSWPotPin 14
#define LeftSWPotPin 23

#define RightHeatPin 18
#define LeftHeatPin 19

#define DC12VReadPin 21
#define LightsReadPin 22

//Global Variables

int DebugMode = 1;

//IO variables

bool RightSWBacklightOut = 0;
bool LeftSWBacklightOut = 0;

int RightHeatOut = 0;
int LeftHeatOut = 0;

int RightSWReading = 0;
int LeftSWReading = 0;

int DC12VReading = 0;
int LightsReading = 0; //perhaps this should be a boolean?

int HeatOutLevelArray[] = {0,50,100,150,200,256}; // the variable to make setting non-linear heating settings easy (0-256)

// math variables
int ScaledRightSWReading = 0;
int ScaledLeftSWReading = 0;

void setup() {
  pinMode(RightSWBacklightPin, OUTPUT);
  pinMode(LeftSWBacklightPin, OUTPUT);
  pinMode(RightSWPotPin, INPUT);
  pinMode(LeftSWPotPin, INPUT);
  pinMode(RightHeatPin, OUTPUT);
  pinMode(LeftHeatPin, OUTPUT);
  pinMode(DC12VReadPin, INPUT);
  pinMode(LightsReadPin, INPUT);
  delay (100);
  if (DebugMode == 1){
  Serial.begin(115200);
    delay(50);
  }
  
}

void loop() {
  ReadInputs (); //read the digital inputs
  WriteToHeaters (); //write the calculated values to the heater Mosfets
  ReadAndSetBacklights (); //as the function name says
  PlotReadings ();
  delay(100);
}

void ReadInputs () { //function to read all the inputs to the system
  RightSWReading = analogRead(RightSWBacklightPin);
  LeftSWReading = analogRead(LeftSWPotPin);
  DC12VReading = analogRead(DC12VReadPin);
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
  }else if (585 >= ScaledRightSWReading && ScaledLeftSWReading >= 535){
    RightHeatOut = HeatOutLevelArray[1];
  }else if (685 >= ScaledRightSWReading && ScaledLeftSWReading >= 586){
    RightHeatOut = HeatOutLevelArray[2];
  }else if (820 >= ScaledRightSWReading && ScaledLeftSWReading >= 686){
    RightHeatOut = HeatOutLevelArray[3];
  }else if (915 >= ScaledRightSWReading && ScaledLeftSWReading >= 821){
    RightHeatOut = HeatOutLevelArray[4];   
  }else if (ScaledRightSWReading >= 916){
    RightHeatOut = HeatOutLevelArray[5];
    }
}

void WriteToHeaters () {
  analogWrite(LeftHeatPin, LeftHeatOut);
  analogWrite(RightHeatPin, RightHeatOut);
}

void ReadAndSetBacklights (){
  //LightsReading = digitalRead(DC12VReadPin); //commented out due to this part of the circuit not being built yet
  if (DC12VReading > 800){
    digitalWrite(LeftSWBacklightPin, HIGH);
    digitalWrite(RightSWBacklightPin, HIGH);
  } else {
    digitalWrite(LeftSWBacklightPin, LOW);
    digitalWrite(LeftSWBacklightPin, LOW);
  }
  
}
  
void PlotReadings () {
  if (DebugMode == 1){
  Serial.print(LeftSWReading);
  Serial.print(" ");
  Serial.print(DC12VReading);
  Serial.print(" ");
  Serial.print(ScaledLeftSWReading);
  Serial.print(" ");
  Serial.print(LeftHeatOut);
  Serial.print(" ");
  Serial.println(LightsReading);
  }
  }
