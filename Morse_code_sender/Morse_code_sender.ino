unsigned long signal_len,t1,t2;
String mCode = "";
byte mCodeLength = 0;
char i;String code = "";

byte elemPause = 0;
int timeUnitLength = 20;
int inputPin = 2;
int ledPin = 13;
int led2 = 12;



void setup() {
  pinMode(inputPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);
  pinMode(led2, OUTPUT);

  Serial.begin(9600);
  Serial.println("SHU Makerspace");
  Serial.println("Morse Sender");
  Serial.println("-----------------------");
  Serial.println("Sacred Heart University");
  Serial.println("Author: Cedric Bleimling");
  Serial.println("Licence CC NC SA");
  Serial.println("-----------------------");
  Serial.println("");
  delay(50);
  

}

void MorseDot(){
  digitalWrite (led2, HIGH);
  delay(timeUnitLength);
  digitalWrite (led2, LOW);
  delay(timeUnitLength);
}

void MorseDash() {
  digitalWrite(led2, HIGH);
  delay(timeUnitLength*3);
  digitalWrite(led2, LOW);
  delay(timeUnitLength);
}

void MorseEndLetter() {
  digitalWrite(led2, LOW);
  delay(timeUnitLength*3);
}

void MorseEndWord() {
  digitalWrite(led2, LOW);
  delay(timeUnitLength*2);
}

void translate(){

  // Take the passed character and use a switch case to find the morse code for that character
  switch(i) {
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
    case '_':
      // Space - We wait 7 time units
      MorseEndWord();
    default:
      //delay(250);
    break;        
  }
  MorseEndLetter();
}

void doString(){
  mCodeLength = mCode.length();
  for(int x = 0; x <= mCodeLength;x++){
    i = mCode.charAt(x);
    translate();
  }
}

char readio()
{
  if (signal_len < 500 && signal_len > 50)
  {
    return '.';                        //if button press less than 0.6sec, it is a dot
  }
  else if (signal_len > 500)
  {
    return '-';                        //if button press more than 0.6sec, it is a dash
  }
}

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
void loop(){

    // When Serial message is sent by pressing enter, we start the "transmission"
  while(Serial.available()) {
    
    char inChar = (char)Serial.read();

    //Space is converted to # for easier analysis
    if(inChar==' '){inChar='_';}

    mCode += inChar;
    if(inChar == '\n') {
      Serial.println(mCode);
      doString();
      mCode="";
    }
  }

  /*Reading the button
  while (digitalRead(inputPin) == HIGH) {}
    t1 = millis();                            //time at button press
    digitalWrite(ledPin, HIGH);               //LED on while button pressed
    while (digitalRead(inputPin) == LOW) {}
    t2 = millis();                            //time at button release
    digitalWrite(ledPin, LOW);                //LED off on button release
    signal_len = t2 - t1;                     //time for which button is pressed
    if (signal_len > 50)                      //to account for switch debouncing
    {
      code += readio();                       //function to read dot or dash
    }
    while ((millis() - t2) < 500)           //if time between button press greater than 0.5sec, skip loop and go to next alphabet
    {     
      if (digitalRead(inputPin) == LOW)
      {
        goto NextDotDash;
      }
    }
    convertor();
    */
}
