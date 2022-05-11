/**
 * Sacred Heart University
 * Morse code receiver
 * 
 * Wiring: 
 *       - 5V - Photoresistor 30K - Pin A1 - 10K resistor - GND
 *       - LCD "VCC" - Pin 5V
 *       - LCD "GND" - Pin GND
 *       - LCD "SDA" - Pin A4
 *       - LCD "SCL" - Pin A5
 *       
 * Author: Cedric Bleimling
 * Licence: CC NC SA
 */

//--------------------
// SETTINGS
//--------------------
int  timeUnitLen = 300; // Speed of the Morse transmition (Duration of the smallest unit in ms - EX: dot is one time unit high + one time unit low)
bool debug = false; // General debug
bool debugSensor = true; // debug the light value threshold
bool debugtimeUnitLen = true; // timeUnitLen debug -> Only displays when touching the potentiometer -> can leave always on
bool debugTiming = false; // debug the morse unit rules (Some clients don't follow the normal rules and their spaces and end of word are too short!)
bool debugAdv = false; // Advanced debug: prints all the decision values
int lcdAdress = 0x3F; // LCD I2C Address (labelled in the back of the screen)
//--------------------




///////////////////////////////////////////////
/* No user changeable code below this point */
///////////////////////////////////////////////

// LCD Screen Library + Initialisation
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(lcdAdress,16,2);
int val = 0; // Light value fron the LDR
int codePtr = 0; // Value used to decode the morse code into a character using the tree.
bool notAnalysed = false; // Keep tracks of wether or not we have treated the previous "State"
bool endOfTrans = true; // Are we still transmitting? Important to print the last letter of the transmission
static unsigned long timer = millis();
bool lightState = false; // state of the light
int lightOnLen = 0; // Used to store how long was the previous "high" state
int lightOffLen = 0; // Used to store how long was the previous "low" state
int threshold = 0; //Will hold the threshold of the photoresistor to differanciate a dash from a dot.

const char MorseTree[] = {'\0','E', 'T', 'I', 'A', 'N', 'M', 'S',
                          'U', 'R', 'W', 'D', 'K', 'G', 'O', 'H',
                          'V', 'F', 'U', 'L', 'A', 'P', 'J', 'B',
                          'X', 'C', 'Y', 'Z', 'Q', '\0','\0','5',
                          '4', '\0','3', '\0','\0','\0','2', '\0',
                          '\0','+', '\0','\0','\0','\0','1', '6',
                          '=', '/', '\0','\0','\0','(', '\0','7',
                          '\0','\0','\0','8', '\0','9', '0', '\0',
                          '\0','\0','\0','\0','\0','\0','\0','\0',
                          '\0','\0','\0','?', '_', '\0','\0','\0',
                          '\0','"', '\0','\0','.', '\0','\0','\0',
                          '\0','@', '\0','\0','\0','\0','\0','\0',
                          '-', '\0','\0','\0','\0','\0','\0','\0',
                          '\0',';', '!', '\0',')', '\0','\0','\0',
                          '\0','\0',',', '\0','\0','\0','\0',':',
                          '\0','\0','\0','\0','\0','\0','\0'
                         };

void getMorse(){    

    // Reading the LDR light value
    val = analogRead(A1);
    if(debugSensor){ Serial.println("Value is :" + String(val) + " - Threshold: " + threshold); }

  // Edge detection based decisions (Going from Low to high and vice versa)
  if (val >= threshold)
  {
    // Detection of a "rising edge" Low->High
    if(!lightState){
     lightOffLen = millis()-timer;
     timer=millis();
     lightState = true;
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght Off: " + String(lightOffLen) + " or " + String(lightOffLen/timeUnitLen) + " units"); }
     lightOffLen = lightOffLen/timeUnitLen; // We count as units of times defined by timeUnitLen
    }
    if(debug){ digitalWrite(13, HIGH); }
  } else {
    // Detection of a "falling edge" High->Low
    if(lightState){
     lightOnLen = millis()-timer;
     timer=millis();
     lightState = false;
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght On: " + String(lightOnLen) + " or " + String(lightOnLen/timeUnitLen) + " units"); }
     lightOnLen = lightOnLen/timeUnitLen; // We count as units of times defined by timeUnitLen
    }
    if(debug){ digitalWrite(13, LOW); }

    // End of Transmission + finish up the last letter
    if(!endOfTrans && ((millis()-timer)/timeUnitLen) >= 10){
          if(debug){ Serial.println("---------------- Letter found: " + String(MorseTree[codePtr]) + " ---------------------"); }
          Serial.print(MorseTree[codePtr]);
          lcd.print(MorseTree[codePtr]);
          codePtr = 0;
          endOfTrans = true;
          if(debug){ Serial.println("End of Transmission"); }
          Serial.println("");
          Serial.println("---------------------------------------------------------------");
    }

  }

    /*** Morse decoding ***/
    // By checking how long the state was "High", 
    // we can deduce if it is a dot or a dash
    if(notAnalysed && !lightState){
      if ( lightOnLen <= 1) {
        Serial.print(".");
        codePtr = (2*codePtr) + 1;
        notAnalysed = false;
      } else if (lightOnLen >= 2) {
        Serial.print("-");
        codePtr = (2*codePtr) + 2;
        notAnalysed = false;
      } else if (lightOnLen >= 3) {
        Serial.println("--------- Morse code issue: *High* for too long ---------");
      }
    }

    // By checking how long the state was "Low", 
    // we can deduce if it is an "end of letter" or "end of word"
    // Screen is cleared if this new message is receivedd more than 50 time units later
    if(notAnalysed && lightState){
        if(lightOffLen <= 1){
          // This is a "regular" off between on for now, we do nothing
          notAnalysed = false;
        } else if(lightOffLen >= 2 && lightOffLen < 5){
          if(debug){ Serial.println("---------------- Letter found: " + String(MorseTree[codePtr]) + " ---------------------"); }
          Serial.print(MorseTree[codePtr]);
          lcd.print(MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
        } else if(lightOffLen >= 5 && lightOffLen < 10){
          if(debug){ Serial.println("---------------- Letter found: " + String(MorseTree[codePtr]) + " ---------------------"); }
          Serial.print(MorseTree[codePtr]);
          lcd.print(MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
          if(debug){ Serial.println(" End of Word "); }
          Serial.print("_");
          lcd.print("_");
        } else if(lightOffLen > 50){
          // Light was off for a long time, we clear the screen for a "New" message
          lcd.clear();
        } 
    }

    // Displays the amounts of time the signal was on/off and current place in the tree
    if(debugAdv){
        Serial.println("lightOnLen: " + String(lightOnLen) + " - lightOffLen: " + String(lightOffLen) + " - timeUnitLen: " + String(timeUnitLen) + " - codePtr: " + String(codePtr));
      }

    if(debug){ digitalWrite(13, LOW); }
}


// threshold value for the photoresistor to differenciate between (light on) and (light off)
void calibrateThreshold()
{
	int initValue = analogRead(A1);
	threshold = initValue*1.25; // Threshoold is 25% more than the current reading of light.
}


void setup()
{ 
  pinMode(13,OUTPUT); // debug led
  pinMode(A1,INPUT); // photoresistor
  Serial.begin(9600);// Start a Serial Connection
  Serial.println("-----------------------");
  Serial.println("Sacred Heart University");
  Serial.println("-----------------------");
  Serial.println("- Morse Code Receiver -");
  Serial.println("-----------------------");
  Serial.println("");
  delay(50);
   
  // set up the LCD's number of columns and rows:
  Wire.begin(); // initialize I2C - required by the I2C-LCD adapter
  lcd.begin(16,2); // Setup the screen size
  lcd.backlight();//To Power ON the back light
  // Print a message to the LCD.
  lcd.setCursor(0,0); // 1st column, 1st row
  lcd.print("SHU ENGINEERING");
  lcd.setCursor(0, 1); // 1st column, 2nd row
  lcd.print("Morse Receiver");
  delay(2000);
  lcd.clear();
  
  // Auto calibrate the photoresistor to ambiant lighting:
  calibrateThreshold();
  
}

void loop()
{
  getMorse();
}
