/**
 * IDEA LAB (Makerspace)
 * Sacred Heart University
 * 
 * Morse code transceiver
 * 
 * Author: Cedric Bleimling
 * Licence: CC NC SA
 */

//--------------------
// USER SETTINGS
//--------------------
int timeUnitLength = 190; //Speed of the morse time unit
int MorseLed = 12; // Morse sender LED
int ControlLed = 13; // Feedback LED
int photoResistorPin = A1; // Photoresistor pin
//--------------------

//--------------------
// DEBUG SETTINGS
//--------------------
bool debug = false; // General debug
bool debugSensor = false; // debug the light value threshold
bool debugtimeUnitLen = false; // timeUnitLen debug -> Only displays when touching the potentiometer -> can leave always on
bool debugTiming = false; // debug the morse unit rules (Some clients don't follow the normal rules and their spaces and end of word are too short!)
bool debugAdv = false; // Advanced debug: prints all the decision values
bool debugSender = false; // Shows the different steps of the morse sending 
bool debugPlotter = false; // Plotter to facilitate the understanding of thresholds. To see it, open the serial plotter in your arduino IDE.
//--------------------

////////////////////////////////////////////////////////////////////////////////////////////////////////////
/* Read below at your own risk */
////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Initialisation of the LCD
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27,16,2);

// Morse tree used for decoding
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


/***** from original receiving part *********/
int val = 0; // Light value fron the LDR
int lightHigh = 0;
int lightLow = 0;
int codePtr = 0;
int previousLen = 200;
bool notAnalysed = false; // Keep tracks of wether or not we have treated the previous "State"
bool endOfTrans = true; // Are we still transmitting? Important to print the last letter of the transmission
static unsigned long timer = millis();
bool lightState = false; // state of the light
int lightOnLen = 0;
int lightOffLen = 0;
int threshold = 0; //Will hold the threshold of the photoresistor to differanciate a dash from a dot.
int lcdPos = 0; // Help track the lcd position to switch lines

/***** from original sender part *********/
// unsigned long signal_len,t1,t2; // For Manual morse code operations
String morseCode = ""; // Containes the morse code to be sent
// byte morseCodeLength = 0;
char i; // BAZINGA can be removed right? (if translate gets the input directly)
String code = ""; // BAZINGA bundle with morseCode?
String message = ""; // Containes the message that needs to be sent
String txBuffer = ""; // Contains the actual state of the LED to transmit (H for high / L for Low and S as a start trigger)

unsigned long ledChangeTimer; // time of the next state change





void getMorse(){    

    // Reading the LDR light value
    val = analogRead(photoResistorPin);
    if(debugSensor){ Serial.println("Value is :" + String(val) + " - Threshold: " + threshold); }

    ////////////////////
  if (val >= threshold)
  {
    // first "high" of the light
    if(!lightState){
     lightOffLen = millis()-timer;
     timer=millis();
     lightState = true;     
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght Off: " + String(lightOffLen) + " or " + String(lightOffLen/timeUnitLength) + " units"); }
     lightOffLen = lightOffLen/timeUnitLength; // We count as units of times defined by timeUnitLength
    }
    if(debug){ digitalWrite(ControlLed, HIGH); }
  } else {
    // first "low" of the light
    if(lightState){
     lightOnLen = millis()-timer;
     timer=millis();
     lightState = false;
     notAnalysed = true;
     endOfTrans = false;
     if(debugTiming){ Serial.println("Lenght On: " + String(lightOnLen) + " or " + String(lightOnLen/timeUnitLength) + " units"); }
     lightOnLen = lightOnLen/timeUnitLength; // We count as units of times defined by timeUnitLength
    }
    if(debug){ digitalWrite(13, LOW); }

    // End of Transmission + finish up the last letter
    if(!endOfTrans && ((millis()-timer)/timeUnitLength) >= 10){
          if(debug){ Serial.println(); Serial.println("--Letter found: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          endOfTrans = true;
          if(debug){ Serial.println(" End of Transmission"); }
          Serial.println("");
          Serial.println("---------------------------------------------------------------");
    }

  }

    /*** Morse decoding ***/

    // When light is off, we can check if short or long signal was given
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
        Serial.println("********* You stay on too long ************");
      }
    }

    // When light is on we check for spaces and ends of words or transmission
    if(notAnalysed && lightState){
        if(lightOffLen <= 1){
          // This is a "regular" off between on for now, we do nothing
          notAnalysed = false;
        } else if(lightOffLen >= 2 && lightOffLen < 5){
          if(debug){ Serial.println(); Serial.println("--Letter found: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
        } else if(lightOffLen >= 5 && lightOffLen < 10){
            if(debug){ Serial.println(); Serial.println("--Letter found: " + String(MorseTree[codePtr]) + " -- Code: " + String(codePtr)); }
          Serial.print(MorseTree[codePtr]);
          lcdPrint((char)MorseTree[codePtr]);
          codePtr = 0;
          notAnalysed = false;
          if(debug){ Serial.println(" End of Word "); }
          Serial.print('#');
          lcdPrint((char)'#');
        } else if(lightOffLen > 50){
          // Light was off for so long, this is a new transmission
          lcd.clear();
		  lcdPos=0; // Tracks the position to switch to the second line
        } 
    }

    if(debugAdv){
        Serial.println("lightOnLen: " + String(lightOnLen) + " - lightOffLen: " + String(lightOffLen) + " - timeUnitLength: " + String(timeUnitLength) + " - Bflag: " + String("") + " - codePtr: " + String(codePtr));
      }

    if(debug){ digitalWrite(13, LOW); }
}

/* Send the morse inputted via Serial
 * 
 * Steps:
 * 		- Reads if there is any new message over Serial
 *		- Translates the message to a morse buffer
 *		- Reads the buffers to decide what to send
*/
void sendMorse()
{
	
	/**************************************************************/
	/* Read the message fronm serial + translate it to morse code */
	/**************************************************************/
    while(Serial.available()) {
		
		// Receiving a new message resets the current morse "Bufffer"
		morseCode = "";
		message = Serial.readStringUntil('\n');
		// replacing spaces by _
		message.replace(" ", "_");
		// Getting rid of caps
		message.toLowerCase();

		// Reading one letter example
		// serialReceived.substring(0,1).toCharArray(commandChar, 2);

		// Translating the message to morse TODO improve this! can do the whole string at once I guess, or include it in translate?
		unsigned int length = message.length();
		for(unsigned int i = 0; i < length; i++)
		{
			translate( message.charAt(i)); // TODO pass the char array in parameter
		}

        Serial.print("Sending message: ");
		Serial.print(message);
		Serial.print(" - Morse: ");
		Serial.println(morseCode);

		// Now that the message is translated, we add # to detext the end of transmission
		// --> TODO check if it cklashes with unknown char, it's late, i changed it three times and lost track lol
		morseCode.concat("#");
    }
	
	/*******************************************************/
	/* Flash the light according to what is in the buffers */
	/*******************************************************/

	// TODO might be more readable if i put all the different cases in the same if/elseif/else instead of nested ones
	// TODO BAZINGA ACtually, let's switch the damn thing to while loops, at least for the transmitting part...

	// If we do not have anything to transmit in the TX buffer we will read the morse buffer
	if( txBuffer.length()==0)
	{
		// If we need to transmit morse
		if( morseCode.length()>0 )
		{
			// get the current morse code to process from the morse buffer
			char currentCode = morseCode.charAt(0);

			// H or L corresponds to 1*timeUnitLength of that state (High or Low)
			// S is a start trigger as we start by removing the first state when starting (TODO more elegant solution)
			switch(currentCode){
			    case '.': 
			      txBuffer = "SHL";
			      break;
			    case '-': 
			      txBuffer = "SHHHL";
			      break;
				 // end of letter
			    case '#': 
			      txBuffer = "SLLL";
			      break;
  				 // end of word
  			    case '%': 
  			      txBuffer = "SLL";
  			      break;
			  }

	  		// Now that this code is added to txBuffer, let's reset timers & remove it
	  		morseCode.remove(0,1);
	  		ledChangeTimer = millis(); //This will trigger the start of the transmition, see below
			
			// Displays the switch from one code to the nexct and new txBuffer content
			if(debugSender){ Serial.print("-- New Code --");}
	
		// No more morse to transmit!
		} else{
			return;
		}
	
	// There is something in the transmit buffer, let's transmit it!
	} else {
		unsigned long currentTime = millis();
		// Are we done transmitting the current state?
		if(currentTime>ledChangeTimer)
		{
			if(debugSender){ Serial.print("current: "); Serial.print(currentTime); Serial.print(" - Change: "); Serial.println(ledChangeTimer); }
			
			// We are done! Let's switch to the next state
			txBuffer.remove(0,1);

			// TODO currently high if error, check better currentState
			if( txBuffer.length()>0 )
			{
				char currentState = txBuffer.charAt(0);
				if(debugSender){ Serial.print("currentState: "); Serial.println(currentState);}
				
				if(currentState=='L')
				{
				    digitalWrite(MorseLed, LOW);
				    digitalWrite(ControlLed, LOW);
					ledChangeTimer = millis()+timeUnitLength;
				}else{
				    digitalWrite(MorseLed, HIGH);
				    digitalWrite(ControlLed, HIGH);
					ledChangeTimer = millis()+timeUnitLength;
				}
				if(debugSender){ Serial.print("txBuffer: "); Serial.print(txBuffer); Serial.print(" - morseCode: "); Serial.println(morseCode); }

			} else {
				// If nothing to send next, we return and the process will restart at the next loop
				return;
			}
		} else {
			// we are still in progress on the current state, nothing to do for now
			return;
		}

	}	
}


// threshold value for the photoresistor to differenciate between (light on) and (light off)
void calibrateThreshold()
{
	int initValue = analogRead(photoResistorPin);
	threshold = initValue*1.25; // Threshoold is 25% more than the current reading of light.
	if(threshold<250){threshold = 250;} // Low light tweak
	// TODO remember max seen and lowest seen and take middle
}

void lcdPrint(char c){
	// Change line on the 1st character
	if(lcdPos==0){ lcd.setCursor(0,0); }
	// Change line on the 16th character
	if(lcdPos==16){ lcd.setCursor(0,1); }
	// Message too long, clear and back to first line
	if(lcdPos==32){ lcd.clear(); lcd.setCursor(0,0); lcdPos=0; }


	// Display the character!
	lcd.print(c);
	lcdPos++;
}

String translate(char* i){

  // Take the passed character and use a switch case to find the morse code for that character
  switch( (char)i) {
    case 'a': 
      morseCode += ".-";
      break;
    case 'b':
		morseCode += "-...";
      break;
    case 'c':
		morseCode += "-.-.";
      break;
    case 'd':
		morseCode += "-..";
      break;
    case 'e':
		morseCode += ".";
      break;
    case 'f':
		morseCode += "..-.";
      break;
    case 'g':
		morseCode += "--.";
      break;
    case 'h':
		morseCode += "....";
      break;
    case 'i':
		morseCode += "..";
      break;
    case 'j':
		morseCode += ".---";
      break;
    case 'k':
		morseCode += "-.-";
      break;
    case 'l':
		morseCode += ".-..";
      break;
    case 'm':
		morseCode += "--";
      break;
    case 'n':
		morseCode += "-.";
      break;
    case 'o':
		morseCode += "---";
      break;
    case 'p':
		morseCode += ".--.";
      break;
    case 'q':
		morseCode += "--.-";
      break;
    case 'r':
		morseCode += ".-.";
      break;
    case 's':
		morseCode += "...";
      break;
    case 't':
		morseCode += "-";
      break;
    case 'u':
		morseCode += "..-";
      break;
    case 'v':
		morseCode += "...-";
      break;
    case 'w':
		morseCode += ".--";
      break;
    case 'x':
		morseCode += "-..-";
      break;
    case 'y':
		morseCode += "-.--";
      break;
    case 'z':
		morseCode += "--..";
      break;
	case '.':
		morseCode += ".-.-.-";
	break;
    case '-':
		morseCode += "-....-";
      break;
    case '(':
  		morseCode += "-.--.";
        break;
    case ')':
			morseCode += "-.--.-";
	      break;
    case '@':
			morseCode += ".--.-.";
	        break;
    case '$':
			morseCode += "...-..-";
			break;
    case '!':
			morseCode += "-.-.--";
			break;
    case '_':
		// space -> end of word 
		morseCode += "..--.-";
    default:
		// unrecognised character
		morseCode += "@";
    break;        
  }
  
  // To identify the end of each letter, we apped a # ()
  morseCode += "#";
  return(morseCode); // BAZINGA TODO WRONG JUST A TEST
}

// TEMP Manual morse button sender
// char readBtn()
// {
//   if (signal_len < 500 && signal_len > 50)
//   {
//     return '.';                        //if button press less than 0.6sec, it is a dot
//   }
//   else if (signal_len > 500)
//   {
//     return '-';                        //if button press more than 0.6sec, it is a dash
//   }
// }

void convertor()
{
  static String letters[] = {".-", "-...", "-.-.", "-..", ".", "..-.", "--.", "....", "..", ".---", "-.-", ".-..", "--", "-.", "---", ".--.", "--.-",
                             ".-.", "...", "-", "..-", "...-", ".--", "-..-", "-.--", "--..", "E"
                            };
  int i = 0;
  if (code == ".-.-.-")
  {
    Serial.print(".");        //for break
  }
  else
  {
    while (letters[i] != "E")  //loop for comparing input code with letters array
    {
      if (letters[i] == code)
      {
        Serial.print(char('A' + i));
        break;
      }
      i++;
    }
    if (letters[i] == "E")
    {
      Serial.println("");  //if input code doesn't match any letter, error
    }
  }
  code = "";                            //reset code to blank string
}


void thresholdPlotter()
{
  int y1 = analogRead(photoResistorPin);
  int y2 = threshold;
  int y3 = 0; // by default we assume the information is 0
  if(y1>y2){y3=1024;} // if value > threshold, we switch it to 1

  Serial.print(y1);
  Serial.print(" "); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.print(y2);
  Serial.print(" "); // a space ' ' or  tab '\t' character is printed between the two values.
  Serial.println(y3);

  delay(100);
}

void setup()
{ 
	pinMode(ControlLed, OUTPUT); // Onboard LED that repeats the same morse code as MorseLed
	pinMode(MorseLed, OUTPUT); // Led that outputs the morse
	pinMode(A1,INPUT); // photoresistor
	
	Serial.begin(9600);// Start a Serial Connection
	Serial.println("-----------------------");
	Serial.println("Sacred Heart University");
	Serial.println("------ IDEA Lab -------");
	Serial.println("-----------------------");
	Serial.println("MorseCode - Transmitter");
	Serial.println("-----------------------");
	Serial.println("");
	delay(50);
  
	// set up the LCD's number of columns and rows:
	Wire.begin();
	lcd.begin(16,2);
	lcd.backlight();//To Power ON the back light
	// Print a message to the LCD.
	lcd.setCursor(0,0);
	lcd.print("SHU MAKERSPACE");
	lcd.setCursor(0, 1);
	lcd.print("Morse Receiver");
	delay(2000);
	lcd.clear();
  
	// Auto calibrate the photoresistor to ambiant lighting:
	calibrateThreshold();  
}

void loop()
{  
  // If we activate the serial plotter, we deactivate everything else 
  // TODO only deactivate any other serial communication!
  if(debugPlotter){
    thresholdPlotter();
  }else{
    // will send morse thru the MorseLed based on Serial input.
    sendMorse();
     
    // Reads the incoming morse code thru the photoresistor
    getMorse();
  }
}

// TODO Stop using Strings everywhere and switch to cstrings!
// If the arduino freezes it might be because of too much memory gruyere from the String usage
