
/******** USER SETTINGS ***********/
int timeUnitLength = 300; //Speed of the morse time unit
int MorseLed = 12; // Morse sender LED
int ControlLed = 13; // Feedback LED
/**********************************/

char inputChar; // Stores the message typed by the user in the serial monitor

void setup() {
  pinMode(ControlLed, OUTPUT);
  pinMode(MorseLed, OUTPUT);

  Serial.begin(9600);
  Serial.println("Morse Sender");
  Serial.println("-----------------------");
  Serial.println("Sacred Heart University");
  Serial.println("-----------------------");
  Serial.println("");  
}

// Turns the led on "one dot".
// The unit of time is based on the general speed -> see timeUnitLength
// One dot is one "time" on -> one "time" off
void MorseDot(){
  digitalWrite (MorseLed, HIGH);
  digitalWrite(ControlLed, HIGH);
  delay(timeUnitLength);
  digitalWrite (MorseLed, LOW);
  digitalWrite(ControlLed, LOW);
  delay(timeUnitLength);
}

// Turns the led on "one dash".
// The unit of time is based on the general speed -> see timeUnitLength
// One dash is three "time" on -> one "time" off
void MorseDash() {
  digitalWrite(MorseLed, HIGH);
  digitalWrite(ControlLed, HIGH);
  delay(timeUnitLength*3);
  digitalWrite(MorseLed, LOW);
  digitalWrite(ControlLed, LOW);
  delay(timeUnitLength);
}


// Indicates the end of morse code for that letter
// The unit of time is based on the general speed -> see timeUnitLength
// It leaves the led off for 3 "time" units
void MorseEndLetter() {
  digitalWrite(MorseLed, LOW);
  delay(timeUnitLength*3);
}

// Indicates the end of that word (adds a space!)
// The unit of time is based on the general speed -> see timeUnitLength
// CAREFUL: the number of "time" units seems to vary, here it is 7.
void MorseEndWord() {
  digitalWrite(MorseLed, LOW);
  delay(timeUnitLength*7);
}

// Translates the character received into the equivalent morse sequence.
// Ex: "a" becomes ".-"
void translate(){

  // transforms upper case in lower case as upper doesn't exist in morse code.
  //inputChar = tolower(inputChar);
  switch(inputChar) {
    case 'a': 
      MorseDot();
      MorseDash();
      break;
    case 'b':
      MorseDash();
      MorseDot();
      MorseDot();
      MorseDot();
      break;
    case 'c':
      MorseDash();
      MorseDot();
      MorseDash();
      MorseDot();
      break;
    case 'd':
      MorseDash();
      MorseDash();
      MorseDot();
      break;
    case 'e':
      MorseDot();
      break;
    case 'f':
      MorseDot();
      MorseDot();
      MorseDash();
      MorseDot();
      break;
    case 'g':
      MorseDash();
      MorseDash();
      MorseDot();
      break;
    case 'h':
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDot();
      break;
    case 'i':
      MorseDot();
      MorseDot();
      break;
    case 'j':
      MorseDot();
      MorseDash();
      MorseDash();
      MorseDash();
      break;
    case 'k':
      MorseDash();
      MorseDot();
      MorseDash();
      break;
    case 'l':
      MorseDot();
      MorseDash();
      MorseDot();
      MorseDot();
      break;
    case 'm':
      MorseDash();
      MorseDash();
      break;
    case 'n':
      MorseDash();
      MorseDot();
      break;
    case 'o':
      MorseDash();
      MorseDash();
      MorseDash();
      break;
    case 'p':
      MorseDot();
      MorseDash();
      MorseDash();
      MorseDot();
      break;
    case 'q':
      MorseDash();
      MorseDash();
      MorseDot();
      MorseDash();
      break;
    case 'r':
      MorseDot();
      MorseDash();
      MorseDot();
      break;
    case 's':
      MorseDot();
      MorseDot();
      MorseDot();
      break;
    case 't':
      MorseDash();
      break;
    case 'u':
      MorseDot();
      MorseDot();
      MorseDash();
      break;
    case 'v':
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDash();
      break;
    case 'w':
      MorseDot();
      MorseDash();
      MorseDash();
      break;
    case 'x':
      MorseDash();
      MorseDot();
      MorseDot();
      MorseDash();
      break;
    case 'y':
      MorseDash();
      MorseDot();
      MorseDash();
      MorseDash();
      break;
    case 'z':
      MorseDash();
      MorseDash();
      MorseDot();
      MorseDot();
      break;
    case '1':
      MorseDot();
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDash();
    case '2':
      MorseDot();
      MorseDot();
      MorseDash();
      MorseDash();
      MorseDash();
    case '3':
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDash();
      MorseDash();
    case '4':
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDash();
    case '5':
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDot();
    case '6':
      MorseDash();
      MorseDot();
      MorseDot();
      MorseDot();
      MorseDot();
    case '7':
      MorseDash();
      MorseDash();
      MorseDot();
      MorseDot();
      MorseDot();
    case '8':
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDot();
      MorseDot();
    case '9':
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDot();
    case '0':
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDash();
      MorseDash();
    case ' ':
      // Space - We wait 7 time units
      MorseEndWord();
    default:
      Serial.print('This character is unsuported:');
      Serial.println(inputChar);
    break;        
  }
  // When we are done dealing with any character
  // we need to signal the letter ending. (3 times units off in morse code)
  // Why? How do we make the difference between "--"->"m" and "---"->"o" VS "-----"->"0"
  MorseEndLetter(); 
}

void loop(){

    // When Serial message is sent by pressing enter, we start the "transmission"
  while(Serial.available()) {
    
    inputChar = (char)Serial.read();

    //Space is converted to _ for easier analysis
    //if(inputChar==' '){inputChar='_';}

    Serial.println(inputChar); // displays the character in the serial monitor
    translate(); // translates the character to morse code + makes the led  blink accordingly.
  }
}
